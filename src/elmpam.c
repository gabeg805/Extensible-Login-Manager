/* *****************************************************************************
 * 
 * Name:    elmpam.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Authenticate username and password using the Pluggable
 *              Authentication Module (PAM).
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmpam.h"
#include "elmdef.h"
#include "elmio.h"
#include "elmx.h"
#include "utility.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <systemd/sd-login.h>

#include <time.h>
#include <utmp.h>
#include <ctype.h>

/* Private functions */
static int    elm_pam_session_open(void);
static int    elm_pam_session_setup_init(char *name, char *dir);
static int    elm_pam_session_setup_files(uid_t uid, gid_t gid);
static int    elm_pam_session_setup_id(char *name, uid_t uid, gid_t gid);
static char * elm_pam_session_cmd(char *xsession);

static int    elm_pam_session_close(void);
static int    elm_pam_session_close_init(void);
static int    elm_pam_end(void);

static int init_env(struct passwd *pw);
static int elm_pam_setenv(char *name, char *value);
static int pam_failure(const char *name, int result);
static int elm_pam_success(char *message, int result);
static int conv(int num_msg, 
                const struct pam_message **msg, 
                struct pam_response **resp, 
                void *appdata_ptr);
static int utmp_record(char *username);
static int utmp_clear(void);

/* Private variables */
static pam_handle_t *pamh = NULL;
static struct utmp utmpr;

/* ************************************************************************** */
/* Open PAM session */
int elm_pam_session_open(void)
{
    elmprintf(LOGINFO, "Preparing to open PAM login session.");

    int result;

    /* Open PAM session */
    result = pam_open_session(pamh, 0);

    if (!elm_pam_success("open session", result)) {
        result = pam_setcred(pamh, PAM_DELETE_CRED);

        if (!elm_pam_success("delete credentials", result)) {
            return -1;
        }

        return -2;
    }

    return 0;
}

/* ************************************************************************** */
/* Session initial setup */
int elm_pam_session_setup_init(char *name, char *dir)
{
    utmp_record(name);
    chdir(dir);

    return 0;
}

/* ************************************************************************** */
/* Setup files that are needed by the authenticated user on login to session */
int elm_pam_session_setup_files(uid_t uid, gid_t gid)
{
    elmprintf(LOGINFO, "%s '%lu:%lu'.",
              "Setting up files for authenticated user", uid, gid);

    /* Xauthority */
    char *xauthority = getenv("XAUTHORITY");

    if (xauthority) {
        if (access(xauthority, F_OK) == 0) {
            chown(xauthority, uid, gid);
        }
    }
    else {
        elmprintf(LOGWARN, "%s: %s.",
                  "Unable to chown Xauthority", "Environment variable not set");
    }

 
    /* ELM log */
    if (access(ELM_LOG, F_OK) == 0) {
        chown(ELM_LOG, uid, gid);
    }
    else {
        elmprintf(LOGWARN, "%s: %s.",
                  "Unable to chown ELM log", "File does not exist.");
    }

    return 0;
}

/* ************************************************************************** */
/* Setup user's ID for session login */
int elm_pam_session_setup_id(char *name, uid_t uid, gid_t gid)
{
    if (initgroups(name, gid) < 0) {
        elmprintf(LOGERR, "Error setting initgroups: %s.", strerror(errno));
        return -1;
    }

    if (setgid(gid) < 0) {
        elmprintf(LOGERR, "Error setting GID: %s.", strerror(errno));
        return -2;
    }

    if (setuid(uid) < 0) {
        elmprintf(LOGERR, "Error setting UID: %s.", strerror(errno));
        return -3;
    }

    return 0;
}

/* ************************************************************************** */
/* Return PAM session command */
char * elm_pam_session_cmd(char *xsession)
{
    static char cmd[128];

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),
             "exec dbus-launch --exit-with-session /usr/bin/%s", xsession);

    elmprintf(LOGINFO, "PAM login session command: %s.", cmd);

    return cmd;
}

/* ************************************************************************** */
/* Read man page of PAM functions for better info, and for initgroups */
/* Could clear ElmSession password field and just pass in that struct? */
int elm_login(char *username, char *xsession, pid_t *parentpid)
{
    elmprintf(LOGINFO, "Logging into PAM session for user.");

    if (elm_pam_session_open() < 0) {
        return -1;
    }

    elmprintf(LOGINFO, "Getting user's password database information.");

    /* Get password entry for user */
    struct passwd *pw = getpwnam(username);

    if (!pw) {
        elmprintf(LOGERR, "%s '%s': %s.",
                  "Unable to get password structure for user", username,
                  strerror(errno));
        return -3;
    }

    elmprintf(LOGINFO, "Setting variables from PW.");

    char  *name  = pw->pw_name;
    char  *shell = pw->pw_shell;
    char  *dir   = pw->pw_dir;
    uid_t  uid   = pw->pw_uid;
    gid_t  gid   = pw->pw_gid;

    endpwent();

    /* Setup and execute user session */
    pid_t pid;

    switch ((pid=fork()))
    {
    /* Child */
    case 0:
        elmprintf(LOGINFO, "Setting up user login with PAM.");

        if (elm_pam_session_setup_init(name, dir) < 0) {
            return -9;
        }

        if (elm_pam_session_setup_files(uid, gid) < 0) {
            return -10;
        }

        if (elm_pam_session_setup_id(name, uid, gid) < 0) {
            return -11;
        }

        init_env(pw);

        char **child_env = pam_getenvlist(pamh);
        int i = 0;

        while (child_env[i] != NULL) {
            elmprintf(LOGINFO, "child_env[%u]: %s", i, child_env[i]);
            ++i;
            if (i > 100)
                break;
        }

        /* Start user X session with xinitrc */
        elmprintf(LOGINFO, "Starting user session.");

        elm_x_load_user_preferences();
        execl(shell, shell, "-c", elm_pam_session_cmd(xsession), NULL);

        elmprintf(LOGERR, "%s: %s.",
                  "Error starting login session", strerror(errno));
        exit(ELM_EXIT_PAM_LOGIN);

    /* Fork error */
    case -1:
        elmprintf(LOGERR, "%s: %s.",
                  "Error during fork to start login session", strerror(errno));
        exit(ELM_EXIT_PAM_LOGIN);

    /* Parent */
    default:
        elmprintf(LOGINFO, "Setting parent PID: '%d'.", pid);
        *parentpid = pid;
        break;
    }

    return 0;
}

/* ************************************************************************** */
/* Close PAM login session */
int elm_pam_session_close(void)
{
    elmprintf(LOGINFO, "Preparing to close PAM login session.");

    int status = 0;
    int result;

    /* Close PAM session */
    result = pam_close_session(pamh, 0);

    if (!elm_pam_success("close session", result)) {
        status = -1;
    }

    /* Remove credentials */
    result = pam_setcred(pamh, PAM_DELETE_CRED);

    if (!elm_pam_success("delete credentials", result)) {
        status = -2;
    }

    elmprintf(LOGINFO, "PAM_SUCCESS returned? %d.", result);

    return status;
}

/* ************************************************************************** */
/* Initialize closing the PAM login session */
int elm_pam_session_close_init(void)
{
    elmprintf(LOGINFO, "Initializing PAM close session.");

    utmp_clear();

    return 0;
}

/* ************************************************************************** */
/* End PAM login session */
int elm_pam_end(void)
{
    elmprintf(LOGINFO, "Preparing to end PAM login session");

    int result;

    /* End PAM session */
    result = pam_end(pamh, PAM_SUCCESS);

    if (!elm_pam_success("end session", result)) {
        pamh = NULL;
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/* Logout of user session */
int elm_logout(void)
{
    elmprintf(LOGINFO, "Preparing to logout of user session.");

    if (elm_pam_session_close_init() < 0) {
        return -1;
    }

    if (elm_pam_session_close() < 0) {
        return -2;
    }

    if (elm_pam_end() < 0) {
        return -3;
    }

    return 0;
}

/* ************************************************************************** */
/* Read man page of PAM functions for better info, and for initgroups */
int elm_authenticate(const char *username, const char *password)
{
    elmprintf(LOGINFO, "Authenticating username and password.");

    const char      *data[2]  = {username, password};
    struct pam_conv  pam_conv = {conv, data};
    int              result;

    /* Start PAM */
    result = pam_start(SERVICE, username, &pam_conv, &pamh);
    if (pam_failure("pam_start", result))
        return 1;

    /* Set items */
    result = pam_set_item(pamh, PAM_TTY, getenv("DISPLAY"));
    if (pam_failure("pam_set_item PAM_TTY", result))
        return 1;
    result = pam_set_item(pamh, PAM_RUSER, "root");
    if (pam_failure("pam_set_item PAM_RUSER", result))
        return 1;
    result = pam_set_item(pamh, PAM_USER, username);
    if (pam_failure("pam_set_item", result))
        return 1;


    /* Authenticate PAM user */
    result = pam_authenticate(pamh, 0);
    if (pam_failure("pam_authenticate", result))
        return 1;

    /* Check if user account is valid */
    result = pam_acct_mgmt(pamh, 0);
    if (pam_failure("pam_acct_mgmt", result))
        return 1;

    /* Establish credentials */
    result = pam_setcred(pamh, PAM_ESTABLISH_CRED);
    if (pam_failure("pam_setcred", result))
        return 1;

    return 0;
}

/* ************************************************************************** */
/* Check if previous PAM command resulted in Success  */
int pam_failure(const char *name, int result)
{
    if (result != PAM_SUCCESS) {
        elmprintf(LOGERR, "%s: %s", name, pam_strerror(pamh, result));
        return 1;
    }

    return 0;
}


/* ************************************************************************** */
/* Check if previous PAM command resulted in Success  */
int elm_pam_success(char *message, int result)
{
    if (result == PAM_SUCCESS) {
        return 1;
    }
    else {
        elmprintf(LOGERR, "%s %s: %s.",
                  "PAM unable to", message, pam_strerror(pamh, result));
        return 0;
    }
}

/* ************************************************************************** */
/* Manager utmp/wtmp login records */
int utmp_record(char *username)
{
    elmprintf(LOG, "Writing utmp login records.");
    utmpr.ut_type = USER_PROCESS;
    utmpr.ut_pid  = getpid();
    snprintf(utmpr.ut_line, sizeof(utmpr.ut_line), "%s", getenv("TTY"));
    snprintf(utmpr.ut_id,   sizeof(utmpr.ut_id),   "%s", getenv("TTYN"));
    time((time_t*)&utmpr.ut_time);
    strcpy(utmpr.ut_user, username);
    memset(utmpr.ut_host, 0, UT_HOSTSIZE);
    utmpr.ut_addr = 0;
    setutent();
    if (pututline(&utmpr) == NULL) {
        elmprintf(LOG, "Unable to write utmp login records.");
        return 1;
    }
    return 0;
}

/* ************************************************************************** */
/* Clear utmp/wtmp login records */
int utmp_clear(void)
{
    elmprintf(LOG, "Clearing utmp login records.");
    utmpr.ut_type = DEAD_PROCESS;
    memset(utmpr.ut_line, 0, UT_LINESIZE);
    utmpr.ut_time = 0;
    memset(utmpr.ut_user, 0, UT_NAMESIZE);
    setutent();
    if (pututline(&utmpr) == NULL) {
        elmprintf(LOG, "Unable to clear utmp login records.");
        return 1;
    }
    endutent();
    return 0;
}

/* ************************************************************************** */
/* Convert login information (username/password) to PAM application data (?) */
static int conv(int num_msg, 
                const struct pam_message **msg,
                struct pam_response **resp,
                void *appdata_ptr)
{
    elmprintf(LOG, "%s", "Converting PAM login information.");

    *resp = calloc(num_msg, sizeof(struct pam_response));
    if ( *resp == NULL )
        return PAM_BUF_ERR;

    int result = PAM_SUCCESS;
    int i;
    for ( i = 0; i < num_msg; i++ ) {
        char *username, *password;
        switch ( msg[i]->msg_style ) {
        case PAM_PROMPT_ECHO_ON:
            username = ((char **) appdata_ptr)[0];
            (*resp)[i].resp = strdup(username);
            break;
        case PAM_PROMPT_ECHO_OFF:
            password = ((char **) appdata_ptr)[1];
            (*resp)[i].resp = strdup(password);
            break;
        case PAM_ERROR_MSG:
            elmprintf(LOG, "%s\n", msg[i]->msg);
            result = PAM_CONV_ERR;
            break;
        case PAM_TEXT_INFO:
            elmprintf(LOG, "%s\n", msg[i]->msg);
            break;
        }

        if ( result != PAM_SUCCESS )
            break;
    }

    if ( result != PAM_SUCCESS ) {
        free(*resp);
        *resp = 0;
    }

    elmprintf(LOG, "%s", "Done converting PAM information.");

    return result;
}

/* ************************************************************************** */
/* Cleanup zombie processes */
void cleanup_child(int signal)
{
    wait(NULL);
}

/* ************************************************************************** */
/* Set environment variables for USER */
int init_env(struct passwd *pw)
{
    elmprintf(LOGINFO, "%s", "Initializing environment variables.");


    elm_pam_setenv("HOME", pw->pw_dir);
    elm_pam_setenv("PWD", pw->pw_dir);
    elm_pam_setenv("SHELL", pw->pw_shell);
    elm_pam_setenv("USER", pw->pw_name);
    elm_pam_setenv("LOGNAME", pw->pw_name);

    elmprintf(LOGINFO, "Checking if XAUTHORITY is set: %s.", getenv("XAUTHORITY"));

    /* char xauthority[64]; */
    /* snprintf(xauthority, sizeof(xauthority), "%s/.Xauthority", pw->pw_dir); */
    /* elm_pam_setenv("XAUTHORITY", xauthority); */

    /* XDG environment variables */
    /* 
     * elm_setenv("XDG_CACHE_HOME", "/home/gabeg/.cache");
     * elm_setenv("XDG_CONFIG_HOME", "/home/gabeg/.config");
     * elm_setenv("XDG_DATA_HOME", "/home/gabeg/.local/share");
     * elm_setenv("XDG_DATA_DIRS", "/usr/local/share/:/usr/share/");
     * elm_setenv("XDG_CONFIG_DIRS", "/etc/xdg");
     * 
     * elm_setenv("XDG_SEAT", "seat0");
     * elm_setenv("XDG_VTNR", getenv("TTYN"));
     * elm_setenv("XDG_SESSION_CLASS", "user");
     * elm_setenv("XDG_SESSION_TYPE", "x11");
     */

    /* int           status; */
    /* char         *session; */
    /* char         *seat; */
    /* unsigned int  vt; */
    /* char          runtimedir[64]; */
    /* if ((status=sd_session_get_vt(session, &vt)) != 0) { */
    /*     elmprintf(LOG, "Error getting loginctl virtual terminal. Setting vtnr to '%s'.", getenv("TTYN")); */
    /*     elm_pam_setenv("XDG_VTNR", getenv("TTYN")); */
    /* } */
    /* else { */
    /*     elmprintf(LOG, "Setting loginctl virtual terminal to '%u'."); */
    /*     char value = ((char)vt)+'0'; */
    /*     elm_pam_setenv("XDG_VTNR", &value); */
    /* } */
    /* if ((status=sd_session_get_seat(session, &seat)) != 0) { */
    /*     elmprintf(LOG, "Error getting loginctl seat. Setting seat to 'seat0'."); */
    /*     elm_pam_setenv("XDG_SEAT", "seat0"); */
    /* } */
    /* else { */
    /*     elmprintf(LOG, "Setting loginctl seat to '%s'.", seat); */
    /*     elm_pam_setenv("XDG_SEAT", seat); */
    /* } */
    /* if ((status=sd_uid_get_display(pw->pw_uid, &session)) < 0) */
    /*     elmprintf(LOG, "Error getting loginctl session."); */
    /* else { */
    /*     elmprintf(LOG, "Setting loginctl session to '%s'.", session); */
    /*     elm_pam_setenv("XDG_SESSION_ID", session); */
    /* } */
    /* snprintf(runtimedir, sizeof(runtimedir), "/run/user/%d", pw->pw_uid); */
    /* elmprintf(LOG, "Setting runtime directory to '%s'.", runtimedir); */
    /* elm_pam_setenv("XDG_RUNTIME_DIR", runtimedir); */

    elmprintf(LOGINFO, "%s", "Done initializing environment variables.");

    return 0;
}


/* ************************************************************************** */
/* Set environment variables */
int elm_pam_setenv(char *name, char *value)
{
    size_t  name_value_len = strlen(name) + strlen(value) + 2;
    char   *name_value     = (char*)malloc(name_value_len);
    snprintf(name_value, name_value_len,  "%s=%s", name, value);

    /* int result = pam_misc_setenv(pamh, name, value, 0); */
    int result = pam_putenv(pamh, name_value);
    int ret    = 0;
    if (pam_failure("pam_putenv", result)) {
        elmprintf(LOGERR,
                  "Error occurred setting pam environment variable: '%s=%s'.",
                  name, value);
        ret = 1;
    }
    if (setenv(name, value, 1) < 0) {
        elmprintf(LOGERR, "Error setting environment variable '%s=%s': %s.", name,
                  value, strerror(errno));
        ret = 1;
    }
    elmprintf(LOGINFO, "Set environment variable: '%s=%s'.", name, value);
    free(name_value);
    return ret;
}
