// 
// CONTRIBUTORS: 
// 
//     * Gabriel Gonzalez (gabeg@bu.edu) 
// 
// 
// LICENSE: 
// 
//     The MIT License (MIT)
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
// OPTIONS:
// 
//     -p, --preview
//         Starts the login manager in preview mode, which does not paint a new X 
//         window.
// 
//     -v, --verbose
//         Print information regarding chronological function execution.
// 
//     -t, --time
//         Time the execution of each function and print out verbose style function
//         information next to the function runtime.
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
//     gabeg Apr 04 2015 <> Not sure if I should initialize globals in a function
//                          or not. I'll keep it just in case.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"

/* char *SERVICE; */
/* char *USERNAME; */
/* char *PASSWORD; */
/* char *SESSION; */
/* char *GLM_LOG; */
/* int  TTYN; */
/* bool INTERFACE; */
/* bool PREVIEW; */
/* bool VERBOSE; */
/* bool BENCHTIME; */

char *SERVICE  = "glm";
char *USERNAME = "User";
char *PASSWORD = "Password";
char *SESSION  = "xterm";
char *GLM_LOG  = "/etc/X11/glm/log/glm.log";
int  TTYN      = 1;
bool INTERFACE = false;
bool PREVIEW   = false;
bool VERBOSE   = false;
bool BENCHTIME = false;



// ////////////////////////////////
// ///// GABE'S LOGIN MANAGER /////
// ////////////////////////////////

// Display Gabe's Login Manager 
int main(int argc, char *argv[]) {
    
    // Read input cli arguments
    cli_parse(argc, argv);
    
    // Initialize global variables
    /* init_globals(); */
    
    // Log program start
    if ( VERBOSE || BENCHTIME ) {
        time_t t;
        time(&t);
        file_log("\nDate: %s", ctime(&t));
    }
    
    // Setup X
    xsetup();
    
    // Start the login loop
    while (true) {
        
        // Authenticate username/password combination
        login_interface(argc, argv);
        
        if ( login(USERNAME, PASSWORD) )
            break;
    }
    
    return 0;
}
