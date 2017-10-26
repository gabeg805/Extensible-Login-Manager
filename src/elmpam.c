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
static int    elm_pam_login_exec(struct passwd *pw, char *xsession, pid_t *parentpid);
static int    elm_pam_session_open(void);
static int    elm_pam_session_setup(struct passwd *pw);
static int    elm_pam_session_setup_misc(char *name , char *dir);
static int    elm_pam_session_setup_files(uid_t uid, gid_t gid);
static int    elm_pam_session_setup_id(char *name, uid_t uid, gid_t gid);
static char * elm_pam_session_cmd(char *xsession);
static struct passwd * elm_session_get_passwd(char *username);
static int    elm_pam_session_end(void);
static int    elm_pam_utmp_write(char *username);
static int    elm_pam_utmp_clear(void);
static int    elm_pam_conversation(int num, const struct pam_message **messages,
                                   struct pam_response **responses, void *data);
static int    elm_pam_success(char *message);

static int init_env(struct passwd *pw);
static int elm_pam_setenv(char *name, char *value);

/* Private variables */
static pam_handle_t *PamHandle = NULL;
static int           PamResult = -1;
static struct utmp utmpr;

/* ************************************************************************** */
/* Read man page of pam functions for better info, and for initgroups */
/* Could clear ElmSession password field and just pass in that struct? */
int elm_login(char *username, char *xsession, pid_t *parentpid)
{
    elmprintf(LOGINFO, "Logging into PAM session for user.");

    struct passwd *pw;

    if (elm_pam_session_open() < 0) {
        return -1;
    }

    if (!(pw=elm_session_get_passwd(username))) {
        return -2;
    }

    elmprintf(LOGINFO, "PWENT: %p.", pw);

    elm_pam_login_exec(pw, xsession, parentpid);

    return 0;
}

/* ************************************************************************** */
/* Execute login command */
int elm_pam_login_exec(struct passwd *pw, char *xsession, pid_t *parentpid)
{
    /* User session login */
    char  *argv[] = {pw->pw_shell, "-c", elm_pam_session_cmd(xsession), NULL};
    pid_t  pid;

    switch ((pid=fork()))
    {
    case 0:
        if (elm_pam_session_setup(pw) < 0) {
            return -3;
        }

        elm_exec(argv[0], argv);
        exit(ELM_EXIT_PAM_LOGIN);
    case -1:
        elmprintf(LOGERRNO, "%s '%s'", "Error during fork to start", argv[0]);
        exit(ELM_EXIT_PAM_LOGIN);
    default:
        break;
    }

    elmprintf(LOGINFO, "Setting parent PID: '%d'.", pid);

    *parentpid = pid;

    return 0;
}

/* ************************************************************************** */
/* Logout of user session */
int elm_logout(void)
{
    elmprintf(LOGINFO, "Preparing to logout of user session.");

    if (elm_pam_utmp_clear() < 0) {
        return -1;
    }

    if (elm_pam_session_end() < 0) {
        return -2;
    }

    return 0;
}

/* ************************************************************************** */
/* Read man page of pam functions for better info, and for initgroups */
int elm_authenticate(const char *username, const char *password)
{
    elmprintf(LOGINFO, "Authenticating username and password.");

    const char      *service  = PROGRAM;
    const char      *data[2]  = {username, password};
    struct pam_conv  pam_conv = {elm_pam_conversation, data};

    /* Start pam */
    PamResult = pam_start(service, username, &pam_conv, &PamHandle);

    if (!elm_pam_success("start service")) {
        return -1;
    }

    /* Have a pam_fail_delay */
    /* PAM_XAUTH_DATA? */

    /* Set items */
    PamResult = pam_set_item(PamHandle, PAM_XDISPLAY, getenv("DISPLAY"));

    if (!elm_pam_success("set DISPLAY item")) {
        return -2;
    }

    PamResult = pam_set_item(PamHandle, PAM_TTY, getenv("TTY"));

    if (!elm_pam_success("set TTY item")) {
        return -3;
    }

    /* PamResult = pam_set_item(PamHandle, PAM_RUSER, "root"); */

    /* if (!elm_pam_success("set RUSER item")) { */
    /*     return -3; */
    /* } */

    PamResult = pam_set_item(PamHandle, PAM_USER, username);

    if (!elm_pam_success("set USER item")) {
        return -4;
    }

    /* Authenticate pam user */
    PamResult = pam_authenticate(PamHandle, 0);
    if (!elm_pam_success("authenticate user")) {
        return -5;
    }

    /* Check if user account is valid */
    PamResult = pam_acct_mgmt(PamHandle, 0);
    if (!elm_pam_success("manage user account")) {
        return -6;
    }

    /* Establish credentials */
    PamResult = pam_setcred(PamHandle, PAM_ESTABLISH_CRED);
    if (!elm_pam_success("establish credentials")) {
        return -7;
    }

    return 0;
}

/* ************************************************************************** */
/* Open pam session */
int elm_pam_session_open(void)
{
    elmprintf(LOGINFO, "Preparing to open PAM login session.");

    PamResult = pam_open_session(PamHandle, 0);

    if (!elm_pam_success("open session")) {
        PamResult = pam_setcred(PamHandle, PAM_DELETE_CRED);

        if (!elm_pam_success("delete credentials")) {
            return -1;
        }

        return -2;
    }

    return 0;
}

/* ************************************************************************** */
/* Setup pam login session */
int elm_pam_session_setup(struct passwd *pw)
{
    elmprintf(LOGINFO, "Setting up user login with PAM.");

    /* Setup miscellaneous items */
    if (elm_pam_session_setup_misc(pw->pw_name, pw->pw_dir) < 0) {
        return -1;
    }

    /* Setup session files */
    if (elm_pam_session_setup_files(pw->pw_uid, pw->pw_gid) < 0) {
        return -2;
    }

    /* Setup user's uid, gid, etc. */
    if (elm_pam_session_setup_id(pw->pw_name, pw->pw_uid, pw->pw_gid) < 0) {
        return -3;
    }

    /* Initialize environment variables */
    init_env(pw);

    /* Load Xresources and Xmodmap configs */
    if (elm_x_load_user_preferences() < 0) {
    }

    return 0;
}

/* ************************************************************************** */
/* Setup miscellaneous items */
int elm_pam_session_setup_misc(char *name, char *dir)
{
    elmprintf(LOGINFO, "Setting up miscellaneous items for the authenticated.");

    /* Write to utmp file */
    if (elm_pam_utmp_write(name) < 0) {
        return -1;
    }

    /* Change directory to authenticated user's home */
    if (chdir(dir) < 0) {
        elmprintf(LOGERR, "%s: %s.",
                  "Unable to change directory", strerror(errno));
        return -2;
    }

    return 0;
}

/* ************************************************************************** */
/* Setup files that are needed by the authenticated user on login to session */
int elm_pam_session_setup_files(uid_t uid, gid_t gid)
{
    elmprintf(LOGINFO, "%s '%lu:%lu'.",
              "Setting up files for the authenticated user", uid, gid);

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
    /* Set initgroups */
    if (initgroups(name, gid) < 0) {
        elmprintf(LOGERR, "Error setting initgroups: %s.", strerror(errno));
        return -1;
    }

    /* Set group ID */
    if (setgid(gid) < 0) {
        elmprintf(LOGERR, "Error setting GID: %s.", strerror(errno));
        return -2;
    }

    /* Set user ID */
    if (setuid(uid) < 0) {
        elmprintf(LOGERR, "Error setting UID: %s.", strerror(errno));
        return -3;
    }

    return 0;
}

/* ************************************************************************** */
/* Return pam session command */
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
/* Return password database entry */
struct passwd * elm_session_get_passwd(char *username)
{
    elmprintf(LOGINFO, "Determining user's password database entry.");

    struct passwd *pw;

    if (!(pw=getpwnam(username))) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to get password database entry for user", username);
        errno = 0;
        return NULL;
    }

    elmprintf(LOGINFO, "PWENT: %p.", pw);

    endpwent();

    elmprintf(LOGINFO, "PWEND: %p.", pw);

    return pw;
}

/* ************************************************************************** */
/* End pam login session */
int elm_pam_session_end(void)
{
    elmprintf(LOGINFO, "Preparing to end PAM login session");

    int status = 0;

    /* Close pam session */
    PamResult = pam_close_session(PamHandle, 0);

    if (!elm_pam_success("close session")) {
        status = -1;
    }

    /* Remove credentials */
    PamResult = pam_setcred(PamHandle, PAM_DELETE_CRED);

    if (!elm_pam_success("delete credentials")) {
        status = -2;
    }

    /* End pam session */
    PamResult = pam_end(PamHandle, PamResult);

    if (!elm_pam_success("end session")) {
        PamHandle = NULL;
        status = -3;
    }

    return status;
}

/* ************************************************************************** */
/* Manager utmp/wtmp login records */
int elm_pam_utmp_write(char *username)
{
    elmprintf(LOG, "Writing utmp login records.");

    /* what is LOGIN_PROCESS */

    /* Define utmp struct */
    utmpr.ut_type = USER_PROCESS;
    utmpr.ut_pid  = getpid();
    utmpr.ut_addr = 0;
    strncpy(utmpr.ut_line, getenv("TTY"),  sizeof(utmpr.ut_line)-1);
    strncpy(utmpr.ut_id,   getenv("TTYN"), sizeof(utmpr.ut_id)-1);
    strncpy(utmpr.ut_user, username,       sizeof(utmpr.ut_user)-1);
    memset(utmpr.ut_host, 0, UT_HOSTSIZE);
    time((time_t*)&utmpr.ut_time);

    /* Write utmp struct to utmp file */
    setutent();

    if (!pututline(&utmpr)) {
        elmprintf(LOGERR, "%s: %s.",
                  "Unable to write utmp login records.", strerror(errno));
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/* Clear utmp/wtmp login records */
int elm_pam_utmp_clear(void)
{
    elmprintf(LOG, "Clearing utmp login records.");

    utmpr.ut_type = DEAD_PROCESS;
    utmpr.ut_time = 0;
    memset(utmpr.ut_line, 0, UT_LINESIZE);
    memset(utmpr.ut_user, 0, UT_NAMESIZE);

    /* Write (cleared) utmp struct to utmp file */
    setutent();

    if (!pututline(&utmpr)) {
        elmprintf(LOGERR, "%s: %s.",
                  "Unable to clear utmp login records.", strerror(errno));
        return 1;
    }

    endutent();

    return 0;
}

/* ************************************************************************** */
/* Communicate between PAM and application to pass in login credentials */
int elm_pam_conversation(int num, const struct pam_message **messages,
                         struct pam_response **responses, void *data)
{
    elmprintf(LOGINFO, "Initiating conversation between PAM and application.");

    /* Check number of responses */
    if ((num <= 0) || (num > PAM_MAX_NUM_MSG)) {
        elmprintf(LOGERR, "Invalid number of responses '%d'.", num);
        return PAM_CONV_ERR;
    }


    /* Allocate memory for response */
    struct pam_response *resp;

    if (!(resp=calloc(num, sizeof(*resp)))) {
        return PAM_BUF_ERR;
    }

    /* Iterate over PAM messages */
    char **credentials = (char**)data;
    char  *username;
    char  *password;
    int    i;

    for (i=0; i < num; i++)
    {
        switch ( messages[i]->msg_style )
        {
        case PAM_PROMPT_ECHO_ON:
            username     = credentials[0];
            resp[i].resp = strdup(username);

            if (!(resp[i].resp)) {
                goto fail;
            }

            break;

        case PAM_PROMPT_ECHO_OFF:
            password     = credentials[1];
            resp[i].resp = strdup(password);

            if (!(resp[i].resp)) {
                goto fail;
            }

            break;

        case PAM_TEXT_INFO:
            elmprintf(LOGINFO, messages[i]->msg);
            break;

        case PAM_ERROR_MSG:
            elmprintf(LOGERR, messages[i]->msg);
            break;

        default:
            goto fail;
        }
    }

    *responses = resp;

    elmprintf(LOGINFO, "PAM conversation completed.");

    return PAM_SUCCESS;

/* Goto failure */
fail:
    elmprintf(LOGERR, "PAM conversation failed.");

    for (i=0; i < num; i++) {
        if (resp[i].resp) {
            memset(resp[i].resp, 0, strlen(resp[i].resp));
            free(resp[i].resp);
        }
    }

    memset(resp, 0, num * sizeof(*resp));
    *responses = 0;

    return PAM_CONV_ERR;
}

/* ************************************************************************** */
/* Check if previous pam command resulted in Success  */
int elm_pam_success(char *message)
{
    if (PamResult == PAM_SUCCESS) {
        return 1;
    }
    else {
        elmprintf(LOGERR, "%s %s: %s.",
                  "PAM unable to", message, pam_strerror(PamHandle, PamResult));
        return 0;
    }
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

    char **envvars = pam_getenvlist(PamHandle);
    char *name;
    char *value;
    char *c;
    int   i;

    for (i=0; envvars[i] && (i < 30); i++) {
        elmprintf(LOGINFO, "envvars[%u]: %s", i, envvars[i]);

        c = strchr(envvars[i], '=');
        if (!c) {
            continue;
        }

        *c    = 0;
        name  = envvars[i];
        value = c+1;

        elmprintf(LOGINFO, "'%s'='%s'", name, value);

        if (!getenv(name)) {
            elmprintf(LOGWARN, "Environment variable '%s' not in user list (%s).", name, value);
            if (elm_setenv(name, value) < 0) {
                elmprintf(LOGERR, "AHHHHH!");
            }
        }

    }


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
    size_t  len    = strlen(name) + strlen(value) + 2;
    char   *envvar = (char*)malloc(len);
    int     ret    = 0;

    snprintf(envvar, len,  "%s=%s", name, value);

    /* int result = pam_misc_setenv(PamHandle, name, value, 0); */
    PamResult = pam_putenv(PamHandle, envvar);

    if (!elm_pam_success("set environment variable")) {
        elmprintf(LOGERR, "%s: '%s=%s'.",
                  "Unable to set pam environment variable", name, value);
        ret = 1;
    }

    if (setenv(name, value, 1) < 0) {
        elmprintf(LOGERR, "Error setting environment variable '%s=%s': %s.", name,
                  value, strerror(errno));
        ret = 1;
    }

    elmprintf(LOGINFO, "Set environment variable: '%s=%s'.", name, value);

    free(envvar);

    return ret;
}
