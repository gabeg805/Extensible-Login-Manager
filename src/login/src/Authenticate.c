// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Authenticate.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "../hdr/Authenticate.h"
// 
// 
// PURPOSE:
// 
//     Authenticate a Username/Password combination and execute the user session 
//     specified in '~/.xinitrc'.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     init_env             - Set environment variables for the authenticated user
// 
//     manage_login_records - Write login to utmp/wtmp 
// 
//     is_pam_success       - Check if previous PAM command resulted in Success
// 
//     conv                 - PAM conversation
// 
//     login                - Authenticate username/password combination with PAM
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Environment Variables
//     * Manage Login Records
//     * Check PAM Function Return Value
//     * PAM Conversation
//     * PAM Login Authentication
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 02 2014 <> created
// 
//     gabeg Aug 10 2014 <> Added program header.
// 
//     gabeg Aug 13 2014 <> Made login method less shady (stopped using 
//                          'su USERNAME -c ...' to login).
// 
//     gabeg Aug 15 2014 <> Added a function to initialize necessary environment 
//                          variables for the authenticated user.
// 
//     gabeg Aug 17 2014 <> Added a function to record user login in UTMP and WTMP.
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries.
// 
//     gabeg Oct 31 2014 <> Modified "init_env" so that the modified "command_line" 
//                          function would work.
// 
//     gabeg Nov 01 2014 <> Changed "command_line" return value to be (char *) so
//                          altered code to reflect that change.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"
#include "../hdr/Authenticate.h"
#include "../hdr/Utility.h"
#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>

#define AWK        "/usr/bin/awk"
#define GREP       "/usr/bin/grep"
#define LOGINCTL   "/usr/bin/loginctl"
#define SESSREG    "/usr/bin/sessreg"
#define UTMP_ADD   "/run/utmp"
#define UTMP_DEL   "/var/run/utmp"
#define WTMP       "/var/log/wtmp"
#define XINITRC    "/etc/X11/glm/src/x/xinitrc"


// Declares
static void init_env(pam_handle_t *pam_handle, struct passwd *pw);
static void manage_login_records(const char *username, char *opt);
static int is_pam_success(int result, pam_handle_t *pamh);
static int conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr);
int login(const char *username, const char *password);

char TTY[6];



// ////////////////////////////////////////////
// ///// INITIALIZE ENVIRONMENT VARIABLES /////
// ////////////////////////////////////////////

// Set environment variables for USER 
static void init_env(pam_handle_t *pam_handle, struct passwd *pw) {
    
    // Define loginctl commands
    char cmd_start[100];
    char seat_cmd[100];
    char session_id_cmd[100];
    
    snprintf(cmd_start, sizeof(cmd_start), "%s | %s %s | %s", LOGINCTL, GREP, pw->pw_name, AWK);
    snprintf(seat_cmd, sizeof(seat_cmd), "%s %s", cmd_start, "'{ print $4 }'");
    snprintf(session_id_cmd, sizeof(session_id_cmd), "%s %s", cmd_start, "'{ print $1 }'");
    
    
    // Define environment variables
    char *seat       = command_line(seat_cmd,       10, 10);
    char *session_id = command_line(session_id_cmd, 10, 10);
    
    char vtnr[3];
    char runtime_dir[100];
    char xauthority[100];
    
    snprintf(vtnr, sizeof(vtnr), "%d", TTY_N);
    snprintf(runtime_dir, sizeof(runtime_dir), "%s/%d", "/run/user", pw->pw_uid);
    snprintf(xauthority, sizeof(xauthority), "%s/%s", pw->pw_dir, ".Xauthority");
    
    // Set environment variables
    setenv("USER", pw->pw_name, 1);
    setenv("SHELL", pw->pw_shell, 1);
    setenv("XDG_VTNR", vtnr, 1);
    setenv("XDG_SEAT", seat, 1);
    setenv("XDG_SESSION_ID", session_id, 1);
    setenv("XDG_RUNTIME_DIR", runtime_dir, 1);
    setenv("XAUTHORITY", xauthority, 1);
    
    // Free memory
    free(seat);
    free(session_id);
}



// ////////////////////////////////
// ///// MANAGE LOGIN RECORDS /////
// ////////////////////////////////

// Manager utmp/wtmp login records
static void manage_login_records(const char *username, char *opt) {
    
    // Execute sessreg
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        
        // Use correct utmp file
        char *UTMP;
        if ( strcmp(opt, "-a") == 0 )
            UTMP = UTMP_ADD;
        else
            UTMP = UTMP_DEL;
        
        // Execute sessreg command
        execl(SESSREG, SESSREG, opt,
              "-w", WTMP, "-u", UTMP,
              "-l", TTY, "-h", "", username, NULL);
    } 
    
    // Wait for process to finish
    int status;
    waitpid(child_pid, &status, 0);
}



// ///////////////////////////////////////////
// ///// CHECK PAM FUNCTION RETURN VALUE /////
// ///////////////////////////////////////////

// Check if previous PAM command resulted in Success 
int is_pam_success(int result, pam_handle_t *pamh) {
    if ( result != PAM_SUCCESS ) {
        file_write(GLM_LOG, "a+", "%s\n", pam_strerror(pamh, result));
        return 0;
    } 
    
    return 1;
}



// ////////////////////////////
// ///// PAM CONVERSATION /////
// ////////////////////////////

// Pam Converstion
static int conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr) {
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



// ////////////////////////////////////
// ///// PAM LOGIN AUTHENTICATION /////
// ////////////////////////////////////

// Authenticate username/password combination with PAM
int login(const char *username, const char *password) {
    
    // Initiate PAM conversation
    pam_handle_t *pam_handle;
    const char *data[2] = {username, password};
    struct pam_conv pam_conv = {conv, data};
    
    // Start PAM
    int result = pam_start(SERVICE, NULL, &pam_conv, &pam_handle);
    if (!is_pam_success(result, pam_handle)) return 0;
    
    // Set PAM items
    snprintf(TTY, sizeof(TTY), "%s%d", "tty", TTY_N);
    
    result = pam_set_item(pam_handle, PAM_USER, username);
    if (!is_pam_success(result, pam_handle)) return 0;
    result = pam_set_item(pam_handle, PAM_TTY, TTY);
    if (!is_pam_success(result, pam_handle)) return 0;
    
    // Authenticate PAM user
    result = pam_authenticate(pam_handle, 0);
    if (!is_pam_success(result, pam_handle)) return 0;
    
    // Check if user account is valid
    result = pam_acct_mgmt(pam_handle, 0);
    if (!is_pam_success(result, pam_handle)) return 0;
    
    // Establish credentials
    result = pam_setcred(pam_handle, PAM_ESTABLISH_CRED);
    if (!is_pam_success(result, pam_handle)) return 0;
    
    // Open PAM session
    result = pam_open_session(pam_handle, 0);
    if (!is_pam_success(result, pam_handle)) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        return 0;
    }
    
    
    // Get mapped user name, PAM may have changed it
    struct passwd *pw = getpwnam(username);
    result = pam_get_item(pam_handle, PAM_USER, (const void **)&username);
    if (result != PAM_SUCCESS || pw == NULL) return 0;
    
    // Clean up zombie processes
    signal(SIGCHLD, cleanup_child);
    
    // Setup and execute user session
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        
        // Check if GLM is in preview mode
        if (PREVIEW) 
            return 1;
        else {
            
            // Log system login start
            file_write(GLM_LOG, "a+", "%s\n", "Setting up user session...");
            
            
            // Kill windows
            system("xwininfo -root -children | grep '  0x' | cut -d' ' -f6 | xargs -n1 xkill -id");
            
            // Add session to utmp/wtmp
            manage_login_records(username, "-a");
                        
            // Change directory and ownership of GLM xinitrc file
            chdir(pw->pw_dir);
            chown(XINITRC, pw->pw_uid, pw->pw_gid);
            chown(GLM_LOG, pw->pw_uid, pw->pw_gid);
                        
            // Set uid and groups for USER
            if (initgroups(pw->pw_name, pw->pw_gid) == -1) return 0;
            if (setgid(pw->pw_gid) == -1)                  return 0;
            if (setuid(pw->pw_uid) == -1)                  return 0;
            
            // Initialize environment variables
            init_env(pam_handle, pw);
            
            // Piece together X session cmd
            size_t sz = strlen(XINITRC) + strlen(SESSION) + 2;
            char cmd[sz];
            snprintf(cmd, sz, "%s %s", XINITRC, SESSION);
            
            
            // Log system login start
            file_write(GLM_LOG, "a+", "%s '%s' %s\n%s '%s'.\n\n", 
                       "User session", SESSION, "is active.", 
                       "Successfully logged in as", username);
            
            
            // Start user X session with xinitrc
            execl(pw->pw_shell, pw->pw_shell, "-c", cmd, NULL);
        }
    }
    
    
    // Wait for child process to finish (wait for logout)
    if (!PREVIEW) {
        int status;
        waitpid(child_pid, &status, 0); 
    }
    
    // Close PAM session
    result = pam_close_session(pam_handle, 0);
    if (!is_pam_success(result, pam_handle)) {
        pam_setcred(pam_handle, PAM_DELETE_CRED);
        return 0;
    }
    
    // Remove credentials
    result = pam_setcred(pam_handle, PAM_DELETE_CRED);
    if (!is_pam_success(result, pam_handle)) return 0;
    
    // End PAM session
    result = pam_end(pam_handle, result);
    pam_handle = 0;
    
    // Delete session from utmp/wtmp
    if (!PREVIEW)
        manage_login_records(username, "-d");    
    
    return 1;
}
