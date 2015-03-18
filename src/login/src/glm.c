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
//     Execute the command:
//         
//         # ./glm
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
//     gabeg Sep 16 2014 <> Removed unneeded libraries
// 
//     gabeg Mar 17 2015 <> Added the stdbool.h library to utilize boolean values. I
//                          know it's a type definition, but it makes the code look
//                          a little cleaner.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"

char *SERVICE  = "glm";
char *USERNAME = "User";
char *PASSWORD = "Password";
char *SESSION  = "xterm";
char *GLM_LOG  = "/etc/X11/glm/log/glm.log";
int TTY_N      = 4;
bool INTERFACE = false;
bool PREVIEW   = false;



// ////////////////////////////////
// ///// GABE'S LOGIN MANAGER /////
// ////////////////////////////////

// Display Gabe's Login Manager 
int main(int argc, char *argv[]) {
    
    // Read input parameters, check for 'Preview' mode
    if ( (argc == 2) && (strcmp(argv[1], "-p") == 0) )
        PREVIEW = true;    
    
    // Setup X
    xsetup();
    
    // Start the login loop
    bool loop = true;
    
    while (loop) {
        
        // Authenticate username/password combination
        login_interface(argc, argv);
        
        if ( login(USERNAME, PASSWORD) )
            loop = false;
    }
    
    // Free memory
    free(USERNAME);
    
    return 0;
}
