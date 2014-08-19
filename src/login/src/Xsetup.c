// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Xsetup.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/Xsetup.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o Xsetup Xsetup.c Config.c FileRW.h
//         $ ./Xsetup
// 
// 
// PURPOSE:
// 
//     Sets up an X server along side a compositing manager to enable transparency.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     start_xserver - Start the X server
// 
//     start_compman - Start the compositing manager
// 
//     xsetup        - Setup X 
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Start X Server
//     * Start Compositing Manager
//     * Setup X 
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 17 2014 <> created
// 
//     gabeg Aug 18 2014 <> Added an algorithm to time the start of the compositing 
//                          manager more approriately
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Xsetup.h"
#include "../hdr/Config.h"
#include "../hdr/FileRW.h"

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// Declares
void start_xserver();
void start_compman();
void xsetup(int preview);



// //////////////////////////
// ///// START X SERVER /////
// //////////////////////////

// Start the X server
void start_xserver() {
    
    // Set the display environment variable
    char *DISPLAY = get_open_display();
    setenv("DISPLAY", DISPLAY, 1);
    
    // Get open tty port
    char VTTY[6];
    snprintf(VTTY, sizeof(VTTY), "%s%d", "vt", get_open_tty());
    
    // Start X server
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        execl(XORG, XORG, "-logverbose", "-logfile", XSERVER_LOG, "-nolisten", "tcp", DISPLAY, "-auth",
              XSERVER_AUTH, VTTY, NULL);
    }
    
    
    // Free display memory
    free(DISPLAY);
}



// /////////////////////////////////////
// ///// START COMPOSITING MANAGER /////
// /////////////////////////////////////

// Start compositing manager (for transparency)
void start_compman() {
    
    // Initialize monotonic clock
    struct timespec start, end; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    
    // Time the logging to X server log file
    char *last, *curr;
    int i = 0, count = 0;
    long BILLION = 1000000000L;
    
    while (1) {
        
        // Calculate time between loops
        clock_gettime(CLOCK_MONOTONIC, &end); 
        int64_t diff  = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec; 
        
        // Get the last line of X server log file
        char cmd[100];
        snprintf(cmd, sizeof(cmd), "%s %s", "/usr/bin/tail -1", XSERVER_LOG);
        char **xcheck = command_line(cmd, sizeof(cmd));
        int num = atoi(xcheck[0]);
        
        // Make sure file is not empty
        if ( num == 1 ) {
            curr = xcheck[1];
            if ( i != 0 && strcmp(last, curr) == 0 )
                ++count;
            last = xcheck[1];
        }
        
        // Once a safe amount of time has elapsed, execute the compositing manager
        if ( (count == 200) || (diff == 2*BILLION) ) {
            pid_t new_pid = fork();
            if ( new_pid == 0 )
                execl(XCOMPMGR, XCOMPMGR, NULL);
            break;
        }
        ++i;
    }
}



// ///////////////////
// ///// SETUP X /////
// ///////////////////
 
// Setup X for login manager
void xsetup(int preview) {
    
    // Start X server when not in preview mode
    if (!preview) 
        start_xserver();
    
    // Start compositing manager
    start_compman();        
    
    // Set background 
    system("/usr/bin/hsetroot -fill /etc/X11/glm/img/wallpapers/night-sky.jpg");
    
    // Cursor and background attributes
    system("/usr/bin/xsetroot -cursor_name left_ptr");    

    // Log that interface is allowed start
    file_write(INTERFACE_LOG, INTERFACE_FLAG, "%s\n");
}
