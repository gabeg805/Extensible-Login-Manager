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
#include "../hdr/Utility.h"

#include <string.h>
#include <stdlib.h>



// ////////////////////////////////
// ///// GABE'S LOGIN MANAGER /////
// ////////////////////////////////

// Display Gabe's Login Manager 
int main(int argc, char *argv[]) {
    
    // Read input parameters, check for 'Preview' mode
    if ( (argc == 2) && (strcmp(argv[1], "-p") == 0) )
        PREVIEW = 1;
    
    // Log program start
    char **date_str = command_line(DATE, 40);
    file_write(GLM_LOG, "a+", "\n%s %s\n%s %d\n\n", "Date:", date_str[1], "Preview:", PREVIEW);
    
    
    // Setup X
    xsetup();
    
    // Start the login loop
    int loop = 1;
    while (loop) {
        
        // Authenticate username/password combination
        login_interface(argc, argv);
        
        if ( login(USERNAME, PASSWORD) )
            loop = 0;
        
        // Free allocated memory
        free(PASSWORD);
        free(USERNAME);
    }
        
    return 0;
}
