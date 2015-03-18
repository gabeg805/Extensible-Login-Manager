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
//     Include the header file:
// 
//         #include "../hdr/Xsetup.h"
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
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Xsetup.h"

// Private functions
static void start_xserver();
static void start_compman();



// //////////////////////////
// ///// START X SERVER /////
// //////////////////////////

// Start the X server
void start_xserver() {
    
    // Log X start
    file_write(GLM_LOG, "a+", "%s\n", "Starting X server...");
    
    // Set the display environment variable
    char DISPLAY[3]  = ":0";
    DISPLAY[1] = get_open_display() + '0';    
    setenv("DISPLAY", DISPLAY, 1);
    
    // Get open tty port
    TTY_N = get_open_tty();
    char VT[6];
    snprintf(VT, sizeof(VT), "%s%d", "vt", TTY_N);
    
    // Start X server
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        execl(XORG, XORG, "-logverbose", "-logfile", XSERVER_LOG, 
              "-nolisten", "tcp", DISPLAY, "-auth",
              XSERVER_AUTH, VT, NULL);
    }
    
    
    // Log X start
    file_write(GLM_LOG, "a+", "%s\n", "X server is active.");
}



// /////////////////////////////////////
// ///// START COMPOSITING MANAGER /////
// /////////////////////////////////////

// Start compositing manager (for transparency)
void start_compman() {
    
    // Log compositing manager start
    file_write(GLM_LOG, "a+", "%s\n", "Starting compositing manager...");    
    
    // Check if composite manager is already running
    size_t runsz = 5;
    char *compcmd = "pgrep -c xcompmgr";
    char *val = command_line(compcmd, runsz, runsz);
    
    if ( atoi(val) != 0 ) {
        free(val);
        return;
    }
    
    // Initialize monotonic clock
    struct timespec start, end; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Time the logging to X server log file
    long BILLION = 1000000000L;
    int count = 0;
    char *last = NULL;
    
    while (1) {
        
        // Calculate time between loops
        clock_gettime(CLOCK_MONOTONIC, &end); 
        int64_t diff  = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec); 
        
        // Check if server file exists
        if ( access(XSERVER_LOG, F_OK) != 0 )
            continue;
        
        // Get the last line of X server log file
        size_t linesz = 100;
        char cmd[linesz]; 
        snprintf(cmd, sizeof(cmd), "%s %s %s", TAIL, "-1", XSERVER_LOG);
        char *xcheck = command_line(cmd, linesz, linesz);
        
        // Make sure file is not empty
        if ( strlen(xcheck) != 0 ) {
            
            // Increment count and free last
            if ( last != NULL ) {
                if ( strcmp(last, xcheck) == 0 ) 
                    ++count;
                free(last);
            }
            
            // Define last
            last = xcheck;
        }
        
        // Once a safe amount of time has elapsed, execute the compositing manager
        if ( (PREVIEW) || (count == 200) || (diff >= 5*BILLION) ) {
            pid_t new_pid = fork();
            if ( new_pid == 0 )
                execl(XCOMPMGR, XCOMPMGR, NULL);
            break;
        }
    }
    
    
    // Log status of composite manager
    file_write(GLM_LOG, "a+", "%s\n", "Composite manager is active.");
    
    // Free memory
    free(val);
}



// ///////////////////
// ///// SETUP X /////
// ///////////////////

// Setup X for login manager
void xsetup() {
    
    // Log program start
    size_t datesz = 40;
    char *date_str = command_line(DATE, datesz, datesz);
    file_write(GLM_LOG, "a+", "\n%s %s%s %d\n\n", "Date:", date_str, "Preview:", PREVIEW);
    
    // Start X server when not in preview mode
    if ( !PREVIEW ) 
        start_xserver();
    
    // Start compositing manager
    start_compman();        
    
    // Set background 
    char hcmd[75];
    snprintf(hcmd, sizeof(hcmd), "%s %s %s", HSETROOT, "-fill", WALLPAPER);
    system(hcmd);
    
    // Cursor and background attributes
    char xcmd[50];
    snprintf(xcmd, sizeof(xcmd), "%s %s %s", XSETROOT, "-cursor_name", "left_ptr");
    system(xcmd);
    
    // Log that interface is allowed start
    INTERFACE = true;
    
    // Free memory
    free(date_str);
}
