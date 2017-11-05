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
#include "elmsession.h"
#include "elmsys.h"
#include "elmx.h"
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <utmp.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_misc.h>
#include <systemd/sd-login.h>

/* Is this needed */
#include <ctype.h>
#include <signal.h>

/* Private functions */
static        int      elm_pam_exec_login(void);
static        int      elm_pam_session_open(void);
static        int      elm_pam_session_setup(struct passwd *pw);
static        int      elm_pam_session_setup_files(uid_t uid, gid_t gid);
static        int      elm_pam_session_setup_id(uid_t uid, gid_t gid);
static        int      elm_pam_session_env(struct passwd *pw);
static        int      elm_pam_session_end(void);
static        int      elm_pam_wtmp_write(void);
static        int      elm_pam_utmp_write(void);
static        int      elm_pam_utmp_clear(void);
static        int      elm_pam_conversation(int num,
                                            const struct pam_message **messages,
                                            struct pam_response **responses,
                                            void *data);
static        char *   elm_pam_get_login_cmd(void);
static struct passwd * elm_pam_get_passwd_entry(void);
static        int      elm_pam_success(char *message);

/* Private variables */
static        pam_handle_t *PamHandle = NULL;
static        ElmLogin     *PamInfo   = NULL;
static        int           PamResult = -1;
static struct utmp          PamUtmp;

/* ************************************************************************** */
/* Start PAM transaction */
int elm_pam_init(ElmLogin *info)
{
    PamInfo = info;
    return 0;
}

/* ************************************************************************** */
/* Authenticate user credentials with PAM */
int elm_pam_auth(void)
{
    const  char     *service      = PROGRAM;
    const  char     *data[2]      = {PamInfo->username, PamInfo->password};
    struct pam_conv  conversation = {elm_pam_conversation, data};
    int              status;

    /* Start pam */
    elmprintf(LOGINFO, "%s '%s'.",
              "Starting PAM transaction for user", PamInfo->username);

    PamResult = pam_start(service, PamInfo->username, &conversation, &PamHandle);

    if (!elm_pam_success("start service")) {
        status = -1;
        goto cleanup;
    }

    /* Set pam items */
    elmprintf(LOGINFO, "Setting PAM items.");

    /* PAM_XAUTH_DATA? */

    /* Set DISPLAY */
    PamResult = pam_set_item(PamHandle, PAM_XDISPLAY, getenv("DISPLAY"));

    if (!elm_pam_success("set DISPLAY item")) {
        status = -2;
    }

    /* Set TTY */
    PamResult = pam_set_item(PamHandle, PAM_TTY, getenv("TTY"));

    if (!elm_pam_success("set TTY item")) {
        status = -3;
    }

    /* Set USER */
    PamResult = pam_set_item(PamHandle, PAM_USER, PamInfo->username);

    if (!elm_pam_success("set USER item")) {
        status = -4;
    }

    /* Authenticate pam user */
    elmprintf(LOGINFO, "Authenticating username and password.");

    PamResult = pam_authenticate(PamHandle, 0);

    if (!elm_pam_success("authenticate user")) {
        status = -5;
        goto cleanup;
    }

    /* Check if user account is valid */
    PamResult = pam_acct_mgmt(PamHandle, 0);

    if (!elm_pam_success("manage user account")) {
        status = -6;
        goto cleanup;
    }

    /* Establish credentials */
    PamResult = pam_setcred(PamHandle, PAM_ESTABLISH_CRED);

    if (!elm_pam_success("establish credentials")) {
        status = -7;
        goto cleanup;
    }

    return 0;

cleanup:
    PamInfo = NULL;
    elm_pam_session_end();
    return status;
}

/* ************************************************************************** */
/* Login and start the user session */
/* Could clear ElmSession password field and just pass in that struct? */
int elm_pam_login(void)
{
    elmprintf(LOGINFO, "Preparing to login and start user session.");

    if (elm_pam_session_open() < 0) {
        return -1;
    }

    return elm_pam_exec_login();
}

/* ************************************************************************** */
/* Execute login command */
int elm_pam_exec_login(void)
{
    struct passwd *pw;

    if (!(pw=elm_pam_get_passwd_entry())) {
        return -1;
    }

    /* Run command */
    char  *cmd    = elm_pam_get_login_cmd();
    char  *argv[] = {pw->pw_shell, "-c", cmd, NULL};
    pid_t  pid;

    switch ((pid=fork()))
    {
    case 0:
        if (elm_pam_session_setup(pw) < 0) {
            return -2;
        }

        elm_sys_exec(argv[0], argv);
        exit(ELM_EXIT_PAM_LOGIN);
    case -1:
        elmprintf(LOGERRNO, "%s '%s'", "Error during fork to start", argv[0]);
        exit(ELM_EXIT_PAM_LOGIN);
    default:
        break;
    }

    /* Wait for session to end */
    elmprintf(LOGINFO, "Waiting for login session to end (pid=%d).", pid);

    int status;

    if (waitpid(pid, &status, 0) == -1) {
        elmprintf(LOGERRNO, "Error while waiting for pid  errored");
        elm_pam_logout();
        return -3;
    }

    /* Check reason for session ending */
    if (WIFEXITED(status)) {
        elmprintf(LOGERR, "Exited with status '%d'.", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)) {
        elmprintf(LOGERR, "Killed by signal '%d'.", WTERMSIG(status));
    }
    else if (WIFSTOPPED(status)) {
        elmprintf(LOGERR, "Stopped by signal '%d'.", WSTOPSIG(status));
    }
    else if (WIFCONTINUED(status)) {
        elmprintf(LOGERR, "Resumed by delivery of SIGCONT.");
    }
    else {
    }

    return 0;
}

/* ************************************************************************** */
/* Logout of user session */
int elm_pam_logout(void)
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
/* Open pam session */
int elm_pam_session_open(void)
{
    elmprintf(LOGINFO, "Preparing to open PAM session.");

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

    if (chdir(pw->pw_dir) < 0) {
        elmprintf(LOGERRNO, "Unable to change directory");
        return -1;
    }

    if (elm_pam_utmp_write() < 0) {
        return -2;
    }

    if (elm_pam_wtmp_write() < 0) {
        return -3;
    }

    if (elm_pam_session_setup_files(pw->pw_uid, pw->pw_gid) < 0) {
        return -4;
    }

    if (elm_pam_session_setup_id(pw->pw_uid, pw->pw_gid) < 0) {
        return -5;
    }

    if (elm_pam_session_env(pw) < 0) {
        return -6;
    }

    if (elm_x_load_user_preferences() < 0) {
        return -7;
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
int elm_pam_session_setup_id(uid_t uid, gid_t gid)
{
    if (initgroups(PamInfo->username, gid) < 0) {
        elmprintf(LOGERRNO, "Error setting initgroups");
        return -1;
    }

    if (setgid(gid) < 0) {
        elmprintf(LOGERRNO, "Error setting GID");
        return -2;
    }

    if (setuid(uid) < 0) {
        elmprintf(LOGERRNO, "Error setting UID");
        return -3;
    }

    return 0;
}

/* ************************************************************************** */
/* Set environment variables for USER */
int elm_pam_session_env(struct passwd *pw)
{
    elmprintf(LOGINFO, "%s", "Initializing environment variables.");

    /* Default environment variables */
    elm_setenv("HOME", pw->pw_dir);
    elm_setenv("PWD", pw->pw_dir);
    elm_setenv("SHELL", pw->pw_shell);
    elm_setenv("USER", pw->pw_name);
    elm_setenv("LOGNAME", pw->pw_name);

    /* Missing environment variables that are set by pam */
    char **envvars = pam_getenvlist(PamHandle);
    char *name;
    char *value;
    char *c;
    int   i;

    for (i=0; envvars[i] && (i < 30); i++) {
        if (!(c=strchr(envvars[i], '='))) {
            continue;
        }

        *c    = 0;
        name  = envvars[i];
        value = c+1;

        if (!getenv(name)) {
            if (elm_setenv(name, value) < 0) {
                continue;
            }
        }
    }

    /* Add additional missing environment variables */
    char *home = getenv("HOME");
    char *ttyn = getenv("TTYN");
    char  cachehome[ELM_MAX_PATH_SIZE];
    char  confighome[ELM_MAX_PATH_SIZE];
    char  datahome[ELM_MAX_PATH_SIZE];
    char  runtimedir[ELM_MAX_PATH_SIZE];

    snprintf(cachehome,  sizeof(cachehome),  "%s/.cache",       home);
    snprintf(confighome, sizeof(confighome), "%s/.config",      home);
    snprintf(datahome,   sizeof(datahome),   "%s/.local/share", home);

    char *xdgvars[][2] = {
        {"XDG_CACHE_HOME",  cachehome},
        {"XDG_CONFIG_HOME", confighome},
        {"XDG_DATA_HOME",   datahome},
        {"XDG_DATA_DIRS",   "/usr/local/share/:/usr/share/"},
        {"XDG_CONFIG_DIRS", "/etc/xdg"},
        {"XDG_SEAT",        "seat0"},
        {"XDG_VTNR",        ttyn},
        {"XDG_RUNTIME_DIR", runtimedir},
        {0, 0},
    };

    for (i=0; xdgvars[i][0]; i++) {
        if (!getenv(xdgvars[i][0])) {
            if (elm_setenv(xdgvars[i][0], xdgvars[i][1])) {
            }
        }
    }

     elm_setenv("XDG_SESSION_CLASS", "user");
     elm_setenv("XDG_SESSION_TYPE", "x11");

    return 0;
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

    PamInfo = NULL;

    return status;
}


/* ************************************************************************** */
/* Manage wtmp login record */
int elm_pam_wtmp_write(void)
{
    elmprintf(LOGINFO, "Writing wtmp login record.");

    updwtmp(_PATH_WTMP, &PamUtmp);

    return 0;
}

/* ************************************************************************** */
/* Manage utmp login record */
int elm_pam_utmp_write(void)
{
    elmprintf(LOGINFO, "Writing utmp login record.");

    /* what is LOGIN_PROCESS */

    /* Define utmp struct */
    PamUtmp.ut_type = USER_PROCESS;
    PamUtmp.ut_pid  = getpid();
    PamUtmp.ut_addr = 0;
    strncpy(PamUtmp.ut_line, getenv("TTY"),     sizeof(PamUtmp.ut_line)-1);
    strncpy(PamUtmp.ut_id,   getenv("TTYN"),    sizeof(PamUtmp.ut_id)-1);
    strncpy(PamUtmp.ut_user, PamInfo->username, sizeof(PamUtmp.ut_user)-1);
    memset(PamUtmp.ut_host, 0, UT_HOSTSIZE);
    time((time_t*)&PamUtmp.ut_time);

    /* Write utmp struct to utmp file */
    if (utmpname(_PATH_UTMP)) {
        elmprintf(LOGERRNO, "Unable to set the utmp file name");
        return -1;
    }

    setutent();

    if (!pututline(&PamUtmp)) {
        elmprintf(LOGERRNO, "Unable to write utmp login record");
        return -2;
    }

    return 0;
}

/* ************************************************************************** */
/* Clear utmp/wtmp login records */
int elm_pam_utmp_clear(void)
{
    elmprintf(LOGINFO, "Clearing utmp login records.");

    PamUtmp.ut_type = DEAD_PROCESS;
    PamUtmp.ut_time = 0;
    memset(PamUtmp.ut_line, 0, UT_LINESIZE);
    memset(PamUtmp.ut_user, 0, UT_NAMESIZE);

    /* Write (cleared) utmp struct to utmp file */
    setutent();

    if (!pututline(&PamUtmp)) {
        elmprintf(LOGERRNO, "Unable to clear utmp login records");
        return -1;
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
/* Return pam session command */
char * elm_pam_get_login_cmd(void)
{
    static char cmd[ELM_MAX_CMD_SIZE];

    memset(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd),
             "exec dbus-launch --exit-with-session /usr/bin/%s",
             PamInfo->xsession);

    elmprintf(LOGINFO, "PAM login session command: %s.", cmd);

    return cmd;
}

/* ************************************************************************** */
/* Return password database entry */
struct passwd * elm_pam_get_passwd_entry(void)
{
    elmprintf(LOGINFO, "Determining user's password database entry.");

    struct passwd *pw;

    if (!(pw=getpwnam(PamInfo->username))) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to get password database entry for user",
                  PamInfo->username);
        errno = 0;
        return NULL;
    }

    endpwent();

    return pw;
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
