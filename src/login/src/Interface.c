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
//     Interface.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "../hdr/Interface.h"
// 
// 
// PURPOSE:
// 
//     Display the login interface.
// 
// 
// OPTIONS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     login_interface - Display the login interface.
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
//                          Utility.c and included Password.c into the main loop
// 
//     gabeg Aug 20 2014 <> Moved the login interface code that displays the 
//                          different modules, into the modules themselves
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries.
// 
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file.
// 
//     gabeg Mar 19 2015 <> Enabled a method to have the application write verbosely 
//                          to the log, in the event that a problem arises.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Interface.h"



// ///////////////////////////////////
// ///// DISPLAY LOGIN INTERFACE ///// 
// ///////////////////////////////////

// Display the login interface
void login_interface(int argc, char *argv[]) {
    
    double bmtime = benchmark_runtime(0);
    
    // Initialize GTK toolkit
    gtk_init(&argc, &argv);
    
    // Display login interface items
    if ( INTERFACE ) {
        
        // Log function start
        if ( VERBOSE )
            file_log("%s: (%s:%d): Displaying login interface...\n", 
                     __FILE__, __FUNCTION__, __LINE__);
        
        // Display interface items
        display_frame();
        display_text_image();
        display_username();
        display_window_manager();
        display_clock();
        display_panel();
        
        // Denote that interface has already begun
        INTERFACE = false;
    }
    
    
    // Display entry box (and define password)
    display_password_entry();
    
    // Log function completion
    if ( VERBOSE )
        file_log("%s: (%s:%d): Done displaying login interface.\n",
                 __FILE__, __FUNCTION__, __LINE__);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
    
    // Begin GTK main loop
    gtk_main();
}
