/* *****************************************************************************
 * 
 * Name:    elmauthenticate.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Authenticate username and password for the Extensible Login
 *              Manager.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmauthenticate.h"
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

#include <time.h>
#include <utmp.h>
#include <ctype.h>

/* Private functions */
static int init_env(struct passwd *pw);
static int set_env(char *name, char *value);
static int pam_failure(const char *name, int result);
static int conv(int num_msg, 
                const struct pam_message **msg, 
                struct pam_response **resp, 
                void *appdata_ptr);
static int utmp_record(void);
static int utmp_clear(void);
static const char * get_username(void);

/* Private variables */
static pam_handle_t *pamh = NULL;
static struct utmp utmpr;


/* ************************************************************************** */
/* Set environment variables for USER */
int init_env(struct passwd *pw)
{
    elmprintf(LOG, "%s", "Initializing environment variables...");

    set_env("HOME", pw->pw_dir);
    set_env("PWD", pw->pw_dir);
    set_env("SHELL", pw->pw_shell);
    set_env("USER", pw->pw_name);
    set_env("LOGNAME", pw->pw_name);

    char xauthority[64];
    snprintf(xauthority,  sizeof(xauthority),  "%s/.Xauthority", pw->pw_dir);
    set_env("XAUTHORITY", xauthority);

    /* /\* Loginctl XDG commands from the config file *\/ */
    /* const char *loginctl = "/usr/bin/loginctl"; */
    /* const char *grep     = "/usr/bin/grep"; */
    /* const char *awk      = "/usr/bin/awk"; */
    /* const char *cmd_fmt  = "%s | %s %s | %s '{ print %s }'"; */
    /* char seat_cmd[MAX_CMD_LEN]; */
    /* char session_id_cmd[MAX_CMD_LEN]; */
    /* snprintf(seat_cmd, sizeof(seat_cmd), cmd_fmt, loginctl, grep, pw->pw_name, */
    /*          awk, "$1"); */
    /* snprintf(seat_cmd, sizeof(seat_cmd), cmd_fmt, loginctl, grep, pw->pw_name, */
    /*          awk, "$4"); */

    /* /\* Define environment variables *\/ */
    /* char seat[MAX_STR_LEN]; */
    /* char session_id[MAX_STR_LEN]; */
    /* char vtnr[MAX_NUM_LEN]; */
    /* char runtime_dir[MAX_LOC_LEN]; */
    /* get_cmd_output(seat,       sizeof(seat),       seat_cmd); */
    /* get_cmd_output(session_id, sizeof(session_id), session_id_cmd); */
    /* snprintf(vtnr,        sizeof(vtnr),        "%d",    TTYN); */
    /* snprintf(runtime_dir, sizeof(runtime_dir), "%s/%d", "/run/user", pw->pw_uid); */

    /* /\* Set environment variables *\/ */
    /* if ( setenv("XDG_VTNR", vtnr, 1) == -1 ) */
    /*     elmprintf(LOG, "Error setting environment variable: %s", strerror(errno)); */
    /* if ( setenv("XDG_SEAT", seat, 1) == -1 ) */
    /*     elmprintf(LOG, "Error setting environment variable: %s", strerror(errno)); */
    /* if ( setenv("XDG_SESSION_ID", session_id, 1) == -1 ) */
    /*     elmprintf(LOG, "Error setting environment variable: %s", strerror(errno)); */
    /* if ( setenv("XDG_RUNTIME_DIR", runtime_dir, 1) == -1 ) */
    /*     elmprintf(LOG, "Error setting environment variable: %s", strerror(errno)); */

    elmprintf(LOG, "%s", "Done initializing environment variables.");
    return 0;
}


/* ************************************************************************** */
/* Set environment variables */
int set_env(char *name, char *value)
{
    size_t  length = strlen(name) + strlen(value) + 2;
    char   *pair   = (char*)malloc(length);
    snprintf(pair, length,  "%s=%s", name, value);
    int result = pam_putenv(pamh, pair);
    int ret    = 0;
    if (pam_failure("pam_putenv", result)) {
        elmprintf(LOG,
                  "Error occurred setting pam environment variable: '%s'.",
                  pair);
        ret = 1;
    }
    if (setenv(name, value, 1) < 0) {
        elmprintf(LOG, "Error setting environment variable: '%s'.", pair);
        ret = 1;
    }
    elmprintf(LOG, "Set environment variable: '%s'.", pair);
    free(pair);
    return ret;
}

/* ************************************************************************** */
/* Check if previous PAM command resulted in Success  */
int pam_failure(const char *name, int result)
{
    if (result != PAM_SUCCESS) {
        elmprintf(LOG, "%s: %s", name, pam_strerror(pamh, result));
        return 1;
    }
    return 0;
}

/* ************************************************************************** */
/* Read man page of PAM functions for better info, and for initgroups */
int elm_authenticate(const char *username, const char *password)
{
    elmprintf(LOG, "Authenticating username and password.");
    const char      *data[2]  = {username, password};
    struct pam_conv  pam_conv = {conv, data};
    int              result;

    /* Start PAM */
    result = pam_start(SERVICE, username, &pam_conv, &pamh);
    if (pam_failure("pam_start", result))
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
/* Read man page of PAM functions for better info, and for initgroups */
int elm_login(const char *session, pid_t *parentpid)
{
    if (PREVIEW) {
        elmprintf(LOG, "Login successful! (Preview Mode)");
        exit(ELM_EXIT_AUTHENTICATE_PREVIEW);
    }

    const char *username = get_username();
    int         result;

    /* Open PAM session */
    result = pam_open_session(pamh, 0);
    if (pam_failure("pam_open_session", result)) {
        result = pam_setcred(pamh, PAM_DELETE_CRED);
        if (pam_failure("pam_open_session->setcred", result))
            return 1;
        return 1;
    }

    /* Get password entry for user */
    struct passwd *pw = getpwnam(username);
    if (pw == NULL) {
        elmprintf(LOG, "Error getting password structure for user '%s': %s",
                  username, strerror(errno));
        return 1;
    }

    /* Setup and execute user session */
    /* signal(SIGCHLD, cleanup_child); */
    const char *xinitrcfile = "/etc/X11/elm/util/xinit/xinitrc";
    pid_t pid = fork();
    switch (pid) {
    case 0:
        elmprintf(LOG, "Setting up user session...");

        /* Begin setup of user session */
        /* Should hide my windows!! */
        /* system("xwininfo -root -children | grep '  0x' | cut -d' ' -f6 | xargs -n1 xkill -id"); */
        utmp_record();
        chdir(pw->pw_dir);
        chown(xinitrcfile, pw->pw_uid, pw->pw_gid);
        chown(ELM_LOG, pw->pw_uid, pw->pw_gid);
        if (initgroups(pw->pw_name, pw->pw_gid) < 0) {
            elmprintf(LOG, "Error setting initgroups: %s.", strerror(errno));
            return 1;
        }
        if (setgid(pw->pw_gid) < 0) {
            elmprintf(LOG, "Error setting GID: %s.", strerror(errno));
            return 1;
        }
        if (setuid(pw->pw_uid) < 0) {
            elmprintf(LOG, "Error setting UID: %s.", strerror(errno));
            return 1;
        }
        init_env(pw);

        /* Start user X session with xinitrc */
        xinitrc();
        elmprintf(LOG, "Starting user session.");
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "exec /bin/bash --login %s %s", xinitrcfile, session);
        execl(pw->pw_shell, pw->pw_shell, "-c", cmd, NULL);
        elmprintf(LOG, "Error starting login shell: %s.", strerror(errno));
        exit(ELM_EXIT_AUTHENTICATE_LOGIN);
        break;
    case -1:
        elmprintf(LOG, "Error forking to start user session: %s.", strerror(errno));
        exit(ELM_EXIT_AUTHENTICATE_LOGIN);
        break;
    default:
        *parentpid = pid;
        break;
    }

    return 0;
}

/* ************************************************************************** */
int elm_logout(void)
{
    utmp_clear();

    elmprintf(LOG, "Closing PAM session");

    /* Close PAM session */
    int result;
    result = pam_close_session(pamh, 0);
    if (pam_failure("pam_close_session", result)) {
        result = pam_setcred(pamh, PAM_DELETE_CRED);
        if (pam_failure("pam_close_session->setcred", result))
            return 1;
        return 1;
    }

    elmprintf(LOG, "Remove PAM credentials");

    /* Remove credentials */
    result = pam_setcred(pamh, PAM_DELETE_CRED);
    if (pam_failure("pam_setcred", result))
        return 1;

    elmprintf(LOG, "End PAM session");

    /* End PAM session */
    result = pam_end(pamh, result);
    if (pam_failure("pam_end", result)) {
        pamh = NULL;
        return 1;
    }

    elmprintf(LOG, "Done PAM");

    return 0;
}

/* ************************************************************************** */
/* Return username */
const char * get_username(void)
{
    const char *username;
    int result = pam_get_item(pamh, PAM_USER, (const void**)&username);
    if (pam_failure("pam_get_item", result))
        return NULL;
    return username;
}

/* ************************************************************************** */
/* Manager utmp/wtmp login records */
int utmp_record(void)
{
    elmprintf(LOG, "Writing utmp login records.");
    utmpr.ut_type = USER_PROCESS;
    utmpr.ut_pid  = getpid();
    snprintf(utmpr.ut_line, sizeof(utmpr.ut_line), "%s", getenv("TTY"));
    snprintf(utmpr.ut_id,   sizeof(utmpr.ut_id),   "%s", getenv("TTYN"));
    time((time_t*)&utmpr.ut_time);
    strcpy(utmpr.ut_user, get_username());
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
    elmprintf(LOG, "%s", "Converting PAM login information...");

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