// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     glm.c
// 
// 
// SYNTAX: 
// 
//     ./glm
// 
// 
// PURPOSE:
// 
//     Display "Gabe's Login Manager", a login manager I created that is extremely 
//     customizable, where any item(s) can be added/removed/modified on the login 
//     screen.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     main - Display Gabe's Login Manager
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Gabe's Login Manager
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 07 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated the header
// 
//     gabeg Aug 14 2014 <> Changed login interface to include password entry box, 
//                          thus changing the main while loop
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Username.h"
#include "../hdr/Password.h"
#include "../hdr/Interface.h"
#include "../hdr/Authenticate.h"
#include "../hdr/FileRW.h"

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <paths.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define   INTERFACE_LOG_FILE   "/etc/X11/glm/log/interface.log"
#define   INTERFACE_FLAG       "TRUE"
#define   XSETUP               "/etc/X11/glm/src/x/Xsetup"


// Declares
void setupX(char *DISPLAY);
void system_login_loop(int argc, char *argv[], int preview);



// //////////////////////////
// ///// SETUP X SERVER ///// 
// //////////////////////////

// Setup the X server
void setupX(char *DISPLAY) {
    
    // Set the display environment variable
    setenv("DISPLAY", DISPLAY, 1);
    
    // Get open tty port
    char TTY[6];
    snprintf(TTY, sizeof(TTY), "%s%d", "tty", get_open_tty());
    
    
    // Setup the X server for logging in
    pid_t child_pid = fork();
    if ( child_pid == 0 )
        execl(XSETUP, XSETUP, DISPLAY, TTY, NULL);
    else {
        int status;
        waitpid(child_pid, &status, 0);
    }
}



// /////////////////////////////
// ///// LOGIN PROMPT LOOP /////
// /////////////////////////////

// Start system login prompt
void system_login_loop(int argc, char *argv[], int preview) {
    
    // Log interface start
    file_write(INTERFACE_LOG_FILE, INTERFACE_FLAG, "%s\n");
    
    // Start the login loop
    int loop = 1;
    while (loop) {
        
        // Authenticate username/password combination
        char *PASSWORD = login_interface(argc, argv);
        char *USERNAME = file_read("/etc/X11/glm/log/user.log");
        
        if ( login(USERNAME, PASSWORD, preview) )
            loop = 0;
        
        // Log that interface has already begun
        file_write(INTERFACE_LOG_FILE, "FALSE", "%s\n");
        
        // Free allocated memory
        free(PASSWORD);
        free(USERNAME);
    }
}



// ////////////////////////////////
// ///// GABE'S LOGIN MANAGER /////
// ////////////////////////////////

// Display Gabe's Login Manager 
int main(int argc, char *argv[]) {
    
    // Start GLM in preview mode
    int preview = 0;
    if ( (argc == 2) && (strcmp(argv[1], "-p") == 0) )
        preview = 1;
    
    
    // Preview parameter not set, it's safe to setup X
    char *DISPLAY;
    if (!preview) {
        DISPLAY = get_open_display();
        setupX(DISPLAY);
    }
    
    // Login to the system
    system_login_loop(argc, argv, preview);
    
    // Free display memory
    if (!preview)
        free(DISPLAY);
    
    return 0;
}
