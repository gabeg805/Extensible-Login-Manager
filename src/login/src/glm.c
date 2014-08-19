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
//     # ./glm
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
#include "../hdr/Config.h"
#include "../hdr/Xsetup.h"
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



// ////////////////////////////////
// ///// GABE'S LOGIN MANAGER /////
// ////////////////////////////////

// Display Gabe's Login Manager 
int main(int argc, char *argv[]) {
    
    // Read input parameters, check for 'Preview' mode
    int preview = 0;
    if ( (argc == 2) && (strcmp(argv[1], "-p") == 0) )
        preview = 1;
    
    // Setup X
    xsetup(preview);
    
    // Start the login loop
    int loop = 1;
    while (loop) {
        
        // Authenticate username/password combination
        char *password = login_interface(argc, argv);
        char *username = file_read(USERNAME_LOG);
        
        if ( login(username, password, preview) )
            loop = 0;
        
        // Free allocated memory
        free(password);
        free(username);
    }
        
    return 0;
}
