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
//     Xsetup.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "Xsetup.h"
// 
// 
// PURPOSE:
// 
//     Sets up an X server along side a compositing manager to enable transparency.
// 
// 
// OPTIONS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     set_open_display - Search for X display lock files and return a display 
//                        that is not locked.
// 
//     set_open_tty     - Return an open TTY port.
// 
//     start_xserver    - Start the X server.
// 
//     start_compman    - Start the compositing manager.
// 
//     xsetup           - Setup X.
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Set Open X Display
//     * Set Open TTY
//     * Start X Server
//     * Start Compositing Manager
//     * Setup X 
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 17 2014 <> Created.
// 
//     gabeg Aug 18 2014 <> Added an algorithm to time the start of the compositing 
//                          manager more approriately.
// 	
//     gabeg Sep 16 2014 <> Removed unneeded libraries.
// 
//     gabeg Oct 25 2014 <> Changed "start_xserver" and "start_compman" functions to 
//                          static and trimmed down the header file so it does not 
//                          contain unnecessary includes. 
// 
//     gabeg Oct 31 2014 <> Changed the way DISPLAY is updated since I altered
//                          "get_open_display". Modified "start_compman" since I
//                          changed "command_line" so that it does not return a 
//                          (char **). Also modified the file logging in "xsetup" so 
//                          that the modified "command_line" would work.
//
//     gabeg Nov 01 2014 <> Testing to see if calls to malloc are expensive by
//                          changing "command_line" to return (char *). It seems
//                          they're not that expensive so I'm keeping "command_line"
//                          as returning (char *).
// 
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file.
// 
//     gabeg Mar 19 2015 <> Utilized the universal-setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
// 
//     gabeg Mar 25 2015 <> Utilized the execute-config-command function.
// 
//     gabeg Apr 03 2015 <> Modified the method to check if X server is up. Now use
//                          XOpenDisplay command to check. Moved this check to 
//                          "start_xserver" function. Also added a timeout for the 
//                          X server startup.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "Xsetup.h"
#include "elysia.h"

// Private functions
static void set_open_display();
static void set_open_tty();
static void start_xserver();
static void start_compman();

char DISPLAY[4];
char VT[6];



// //////////////////////////////
// ///// SET OPEN X DISPLAY ///// 
// //////////////////////////////

// Return an open display in the form ':0'
static void set_open_display() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Searching for an open display...", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    // Used display file name
    char file[15];
    bool open = false;
    int d;
    
    // Loop through possible open displays
    for ( d=0; d < 10; d++ ) {
        
        // Check if the file exists (display in use, if exists)
        snprintf(file, sizeof(file), "/tmp/.X%d-lock", d);
        
        if ( access(file, F_OK) ) {
            open = true;
            break;
        }
    }
    
    // Found open display
    if ( open ) {
        
        // Set the display environment variable
        snprintf(DISPLAY, sizeof(DISPLAY), ":%d", d);
        setenv("DISPLAY", DISPLAY, 1);
        
        // Log function completion
        if ( VERBOSE )
            file_log("Done.\n%s: (%s:%d): Display '%s' is open.\n", 
                     __FILE__, __FUNCTION__, __LINE__, DISPLAY);
    } else {
        
        // Log function end
        if ( VERBOSE )
            file_log("Error.\n%s (%s:%d): An open display could not be found. Exiting...\n", 
                     __FILE__, __FUNCTION__, __LINE__);
        exit(1);
    }
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// ////////////////////////
// ///// SET OPEN TTY /////
// ////////////////////////

// Return an open TTY port
// * Find a better way to do this
static void set_open_tty() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Determining TTY...", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    // Choosing 7 as the default virtual terminal
    TTYN = 7;
    snprintf(VT, sizeof(VT), "%s%d", "vt", TTYN);
    
    // Log function completion
    if ( VERBOSE )
        file_log("Done.\n%s: (%s:%d): TTY number is '%d'.\n", 
                 __FILE__, __FUNCTION__, __LINE__, TTYN);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));

}



// //////////////////////////
// ///// START X SERVER /////
// //////////////////////////

// Start the X server
static void start_xserver() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Starting X server...\n", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    // Set the display environment variable
    set_open_display();
    
    // Get open tty port
    set_open_tty();
    
    // Start X server
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        execl(XORG, XORG, "-logverbose", "-logfile", XSERVER_LOG, 
              "-nolisten", "tcp", DISPLAY, "-auth",
              XSERVER_AUTH, VT, NULL);
    }
    
    // Timer variables for X startup
    struct timespec t_start, t_end;
    double start, end;
    int timeout;
    
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    start   = (double)(t_start.tv_sec + (t_start.tv_nsec / 1e9));
    timeout = read_config_int(X_CONFIG, "timeout");
    
    // Wait until X server is up
    while ( XOpenDisplay(NULL) == NULL ) {
        
        clock_gettime(CLOCK_MONOTONIC, &t_end);        // X server startup timeout
        end = (double)(t_end.tv_sec + (t_end.tv_nsec / 1e9));
        
        if ( (end-start) >= timeout ) {
            file_log("%s: (%s:%d): Timeout reached: %s (%d sec). Elysia exited.",
                     __FILE__, __FUNCTION__, __LINE__, 
                     "X server timed out", timeout);
            exit(1);
        }
    }
    
    // Log function completion
    if ( VERBOSE )
        file_log("%s: (%s:%d): X server is active.\n", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// /////////////////////////////////////
// ///// START COMPOSITING MANAGER /////
// /////////////////////////////////////

// Start compositing manager (for transparency)
static void start_compman() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Starting compositing manager...", 
                 __FILE__, __FUNCTION__, __LINE__);

    
    // Check if composite manager is already running (not yet done)
    if ( is_running(XCOMPMGR) ) {
        if ( VERBOSE )
            file_log("Already running.\n");
        return;
    }
    
    // Start compositing manager
    pid_t pid = fork();
    if ( pid == 0 ) 
        execl(XCOMPMGR, XCOMPMGR, NULL);
    
    // Log function completion
    if ( VERBOSE )
        file_log("Done.\n");
    
    if ( BENCHTIME ) 
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// ///////////////////
// ///// SETUP X /////
// ///////////////////

// Setup X for login manager
void xsetup() {
    
    double bmtime = benchmark_runtime(0);
    
    // Start X server when not in preview mode
    if ( !PREVIEW ) 
        start_xserver();
    
    // Start compositing manager
    start_compman();
    
    // Set background 
    exec_config_cmd(X_CONFIG, 1);
    
    // Cursor and background attributes
    exec_config_cmd(X_CONFIG, 2);
    
    // Log that interface is allowed start
    INTERFACE = true;
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}
