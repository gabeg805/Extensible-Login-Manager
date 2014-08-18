// Includes
#include "../hdr/Xsetup.h"
#include "../hdr/FileRW.h"

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define   XORG                "/usr/bin/Xorg"
#define   XCOMPMGR            "/usr/bin/xcompmgr"
#define   XSERVER_LOG_FILE    "/etc/X11/glm/log/xserver.log"
#define   XSERVER_AUTH_FILE   "/etc/X11/glm/log/glm.auth"

#define BILLION 1000000000L 


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
        execl(XORG, XORG, "-logverbose", "-logfile", XSERVER_LOG_FILE, "-nolisten", "tcp", DISPLAY, "-auth",
              XSERVER_AUTH_FILE, VTTY, NULL);
    }
    
    
    // Free display memory
    free(DISPLAY);
}



// /////////////////////////////////////
// ///// START COMPOSITING MANAGER /////
// /////////////////////////////////////

// Start compositing manager (for transparency)
void start_compman() {

    struct timespec start, end; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    char *last, *curr;
    int i = 0, count = 0;
    
    while (1) {
        clock_gettime(CLOCK_MONOTONIC, &end); 
        int64_t diff  = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec; 
        
        char cmd[100];
        snprintf(cmd, sizeof(cmd), "%s %s", "/usr/bin/tail -1", XSERVER_LOG_FILE);
        char **xcheck = command_line(cmd, sizeof(cmd));
        int num = atoi(xcheck[0]);
        
        if ( num == 1 ) {
            curr = xcheck[1];
            if ( i != 0 && strcmp(last, curr) == 0 )
                ++count;
            last = xcheck[1];
        }
        
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
    
    // Setup X regularly when not in preview mode
    if (!preview) {
        start_xserver();
        start_compman();        
    }    
    
    // Set background 
    system("/usr/bin/hsetroot -fill /etc/X11/glm/img/wallpapers/night-sky.jpg");
    
    // Cursor and background attributes
    system("/usr/bin/xsetroot -cursor_name left_ptr");    
}
