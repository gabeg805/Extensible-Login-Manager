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
//     gabeg Mar 19 2015 <> Utilized the universal setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
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

char DISPLAY[4];
char VT[6];



// //////////////////////////////
// ///// SET OPEN X DISPLAY ///// 
// //////////////////////////////

// Return an open display in the form ':0'
void set_open_display() {
    
    // Log function start
    file_write(GLM_LOG, "a+", "%s: (%s:%d): Searching for an open display...", 
               __FILE__, __FUNCTION__, __LINE__);
    
    // Used display file name
    char *xtmp = "/tmp/.X";
    char *xlock = "-lock";
    char file[15];
    
    // Loop through possible open displays
    int d;
    bool open = false;
    
    for ( d=0; d < 10; d++ ) {
        
        // Piece together the file name
        snprintf(file, sizeof(file), "%s%d%s", xtmp, d, xlock);
        
        // Check if the file exists (display in use, if exists)
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
        file_write(GLM_LOG, "a+", "Done.\n%s: (%s:%d): Display '%s' is open.\n", 
                   __FILE__, __FUNCTION__, __LINE__, DISPLAY);
    } else {
        
        // Log function end
        file_write(GLM_LOG, "a+", 
                   "Error.\n%s (%s:%d): An open display could not be found. Exiting...\n", 
                   __FILE__, __FUNCTION__, __LINE__);
        exit(1);
    }
}



// ////////////////////////
// ///// SET OPEN TTY /////
// ////////////////////////

// Return an open TTY port
// * Find a better way to do this
void set_open_tty() {
    
    // Log function start
    file_write(GLM_LOG, "a+", "%s: (%s:%d): Determining TTY...", 
               __FILE__, __FUNCTION__, __LINE__);
    
    // Choosing 7 as the default virtual terminal
    TTYN = 7;
    snprintf(VT, sizeof(VT), "%s%d", "vt", TTYN);
    
    // Log function completion
    file_write(GLM_LOG, "a+", "Done.\n%s: (%s:%d): TTY number is '%d'.\n", 
               __FILE__, __FUNCTION__, __LINE__, TTYN);
}



// //////////////////////////
// ///// START X SERVER /////
// //////////////////////////

// Start the X server
void start_xserver() {
    
    // Log function start
    file_write(GLM_LOG, "a+", "%s: (%s:%d): Starting X server...\n", 
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
    
    // Log function completion
    file_write(GLM_LOG, "a+", "%s: (%s:%d): X server is active.\n", 
               __FILE__, __FUNCTION__, __LINE__);
}



// /////////////////////////////////////
// ///// START COMPOSITING MANAGER /////
// /////////////////////////////////////

// Start compositing manager (for transparency)
void start_compman() {
    
    // Log function start
    file_write(GLM_LOG, "a+", "%s: (%s:%d): Starting compositing manager...", 
               __FILE__, __FUNCTION__, __LINE__);

    
    // Check if composite manager is already running
    size_t runsz = 5;
    char *compcmd = "pgrep -c xcompmgr";
    char *val = command_line(compcmd, runsz, runsz);
    
    if ( atoi(val) != 0 ) {
        
        // Log status of composite manager
        file_write(GLM_LOG, "a+", "Already running.\n");
        
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
    
    // Log function completion
    file_write(GLM_LOG, "a+", "Done.\n");
    
    // Free memory
    free(val);
}



// ///////////////////
// ///// SETUP X /////
// ///////////////////

// Setup X for login manager
void xsetup() {
    
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
}
