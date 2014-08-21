// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Interface.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/Interface.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o Interface Interface.c Username.c Password.c Clock.c Frame.c TextImage.c WindowManager.c Panel.c Transparency.c FileRW.c `pkg-config gtk+-3.0 --cflags --libs`
//         $ ./Interface
// 
// 
// PURPOSE:
// 
//     Display the login interface.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     login_interface - Display the login interface
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Display Login Interface
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 07 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
// 
//     gabeg Aug 14 2014 <> Updated gcc command to execute in the header to include
//                          FileRW.c and included Password.c into the main loop
// 
//     gabeg Aug 20 2014 <> Moved the login interface code that displays the 
//                          different modules, into the modules themselves
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Interface.h"
#include "../hdr/Config.h"
#include "../hdr/Username.h"
#include "../hdr/Password.h"
#include "../hdr/Clock.h"
#include "../hdr/Frame.h"
#include "../hdr/TextImage.h"
#include "../hdr/WindowManager.h"
#include "../hdr/Panel.h"
#include "../hdr/Transparency.h"
#include "../hdr/FileRW.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Declares
void login_interface(int argc, char *argv[]);



// ///////////////////////////////////
// ///// DISPLAY LOGIN INTERFACE ///// 
// ///////////////////////////////////

// Display the login interface
void login_interface(int argc, char *argv[]) {
    
    // Initialize GTK toolkit
    gtk_init(&argc, &argv);
    
    // Define username
    USERNAME = file_read(USERNAME_LOG);
    
    
    // Display login interface items
    if ( INTERFACE ) {
        
        // Log interface display
        file_write(GLM_LOG, "a+", "%s\n", "Displaying login interface...");
        
        // Display interface items
        display_clock();
        display_frame();
        display_text_image();
        display_window_manager();        
        display_username();
        display_panel();
        
        // Denote that interface has already begun
        INTERFACE = 0;
    }
    
    
    // Display entry box (and define password)
    display_password_entry();
    
    // Begin GTK main loop
    gtk_main();
    
    
    // Log interface display is done
    file_write(GLM_LOG, "a+", "%s\n", "Login interface finished.");
}
