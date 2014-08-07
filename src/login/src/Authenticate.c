// Includes
#include "../hdr/CommandLine.h"

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVICE_NAME   "shart"
#define XTTY           "tty7"


// Declares
int is_pam_success(int result, pam_handle_t *pamh);
int login(const char *username, const char *password);
int conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr);



// /////////////////////////////////////
// ///// CHECK PAM FUNCTION RETURN /////
// /////////////////////////////////////

// Check if previous PAM command resulted in Success 
int is_pam_success(int result, pam_handle_t *pamh) {
    
    // Check result of the last executed PAM command
    if ( result != PAM_SUCCESS ) {
        printf("%s\n", pam_strerror(pamh, result));
        return 0;
    } 
    
    return 1;
}



// //////////////////////////////
// ///// PAM AUTHENTICATION /////
// //////////////////////////////

// Authenticate username/password combination with PAM
int login(const char *username, const char *password) {
    
    // Initiate PAM conversation
    pam_handle_t *pam_handle;
    const char *data[2] = {username, password};
    struct pam_conv pam_conv = {conv, data};
    
    int result = pam_start(SERVICE_NAME, NULL, &pam_conv, &pam_handle);
    if (!is_pam_success(result, pam_handle)) return 1;
    
    result = pam_set_item(pam_handle, PAM_USER, username);
    if (!is_pam_success(result, pam_handle)) return 1;
    
    result = pam_set_item(pam_handle, PAM_TTY, XTTY);
    if (!is_pam_success(result, pam_handle)) return 1;
    
    result = pam_authenticate(pam_handle, 0);
    if (!is_pam_success(result, pam_handle)) return 1;
    
    result = pam_acct_mgmt(pam_handle, 0);
    if (!is_pam_success(result, pam_handle)) return 1;
    
    result = pam_setcred(pam_handle, PAM_ESTABLISH_CRED);
    if (!is_pam_success(result, pam_handle)) return 1;
    
    result = pam_open_session(pam_handle, 0);
    if (!is_pam_success(result, pam_handle)) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        return 1;
    }
    
    /* get mapped user name; PAM may have changed it */
    struct passwd *pw = getpwnam(username);
    result = pam_get_item(pam_handle, PAM_USER, (const void **)&username);
    if (result != PAM_SUCCESS || pw == NULL) return 1;
    
    
    /* export PAM environment */
    char **pam_envlist, **pam_env;
    if ((pam_envlist = pam_getenvlist(pam_handle)) != NULL) {
        for (pam_env = pam_envlist; *pam_env != NULL; ++pam_env) {
            putenv(*pam_env);
            free(*pam_env);
        }
        free(pam_envlist);
    }
    
    
    pid_t child_pid = fork();
    if (child_pid == 0) {
        chdir(pw->pw_dir);
        
        system("xwininfo -root -children | grep '  0x' | cut -d' ' -f6 | xargs -n1 xkill -id");
        
        char cmd[100];
        char *session = command_line("tail -1 /etc/X11/glm/log/session.log");
        snprintf(cmd, sizeof(cmd), "%s %s %s", "su gabeg -c 'exec /bin/bash -login /etc/X11/glm/src/x/xinitrc", session, "'");
        execl(pw->pw_shell, pw->pw_shell, "-c", cmd, NULL);
    }
    
    
    // Wait for child process to finish (wait for logout)
    int status;
    waitpid(child_pid, &status, 0); // TODO: Handle errors
    
    result = pam_close_session(pam_handle, 0);
    if (!is_pam_success(result, pam_handle)) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        return 1;
    }
    
    result = pam_setcred(pam_handle, PAM_DELETE_CRED);
    if (!is_pam_success(result, pam_handle)) return 1;
        
    result = pam_end(pam_handle, result);
    pam_handle = 0;
    
    return 0;
}



// ////////////////////////////
// ///// PAM CONVERSATION /////
// ////////////////////////////

// Pam Converstion
int conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
    int i;
    
    *resp = calloc(num_msg, sizeof(struct pam_response));
    if (*resp == NULL) {
        return PAM_BUF_ERR;
    }
    
    int result = PAM_SUCCESS;
    for (i = 0; i < num_msg; i++) {
        char *username, *password;
        switch (msg[i]->msg_style) {
        case PAM_PROMPT_ECHO_ON:
            username = ((char **) appdata_ptr)[0];
            (*resp)[i].resp = strdup(username);
            break;
        case PAM_PROMPT_ECHO_OFF:
            password = ((char **) appdata_ptr)[1];
            (*resp)[i].resp = strdup(password);
            break;
        case PAM_ERROR_MSG:
            fprintf(stderr, "%s\n", msg[i]->msg);
            result = PAM_CONV_ERR;
            break;
        case PAM_TEXT_INFO:
            printf("%s\n", msg[i]->msg);
            break;
        }
        if (result != PAM_SUCCESS) {
            break;
        }
    }

    if (result != PAM_SUCCESS) {
        free(*resp);
        *resp = 0;
    }

    return result;
}
