/* *****************************************************************************
 * 
 * Name:    xsetup.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Setup the X server and all its components (composite manager, 
 *              background, etc.) for the Elysia Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "xserver.h"
#include "elyglobal.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>

/* Private functions */
static void set_open_display();
static void set_open_tty();
static void start_xserver();
static void start_compman();

char DISPLAY[4];
char VT[6];



/* ******************* */
/* ***** SETUP X ***** */
/* ******************* */

/* Setup X for login manager */
void xsetup() {
    TRACE(stdout, "%s", "Setting up X environment...");

    if ( !PREVIEW )
        start_xserver();
    start_compman();
    exec_config_cmd(X_CONFIG, 1);
    exec_config_cmd(X_CONFIG, 2);

    TRACE(stdout, "%s", "Done with X setup.");
}



/* ************************** */
/* ***** START X SERVER ***** */
/* ************************** */

/* Start the X server */
/* Note: I added a check if Xorg is already running, but if multiple instances 
 * can be run, it needs to be removed 
 */
static void start_xserver() {
    TRACE(stdout, "%s", "Starting X server...");

    /* Check if X server is already running */
    if ( is_running(basename(XORG)) ) {
        TRACE(stdout, "%s already running.", XORG);
        return;
    }

    /* Start X server */
    set_open_display();
    set_open_tty();
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
        TRACE(stdout, "Executing %s command...", XORG);
        execl(XORG, XORG, "-logverbose", "-logfile", XSERVER_LOG, 
              "-nolisten", "tcp", DISPLAY, "-auth",
              XSERVER_AUTH, VT, 0);
    }

    struct timespec t_start, t_end;
    double start, end;
    int timeout;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    start   = (double)(t_start.tv_sec + (t_start.tv_nsec / 1e9));
    timeout = read_config_int(X_CONFIG, "timeout");

    /* Wait until X server is up */
    while ( XOpenDisplay(0) == 0 ) {
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        end = (double)(t_end.tv_sec + (t_end.tv_nsec / 1e9));
        if ( (end-start) >= timeout ) {
            TRACE(stdout, "X server timed out (%d sec). Exiting.", timeout);
            exit(1);
        }
    }

    TRACE(stdout, "%s", "X server now active.");
}

/* ************************************* */
/* ***** START COMPOSITING MANAGER ***** */
/* ************************************* */

/* Start compositing manager (for transparency) */
static void start_compman() {
    TRACE(stdout, "%s", "Starting compositing manager...");
    
    /* Check if composite manager is already running */
    if ( is_running(basename(XCOMPMGR)) ) {
        TRACE(stdout, "%s already running.", XCOMPMGR);
        return;
    }

    /* Start compositing manager */
    pid_t pid = fork();
    if ( pid == 0 ) 
        execl(XCOMPMGR, XCOMPMGR, NULL);

    TRACE(stdout, "%s", "Done with composite manager startup.");
}



/* ****************************** */
/* ***** SET OPEN X DISPLAY *****  */
/* ****************************** */

/* Return an open display (i.e. ':0') */
static void set_open_display() {
    TRACE(stdout, "%s", "Searching for an open display...");

    bool open = false;
    char file[15];
    int d;

    /* Determine an open display */
    for ( d=0; d < 10; d++ ) {
        snprintf(file, sizeof(file), "/tmp/.X%d-lock", d);
        if ( access(file, F_OK) ) {
            open = true;
            break;
        }
    }

    /* Open display found */
    if ( open ) {
        snprintf(DISPLAY, sizeof(DISPLAY), ":%d", d);
        setenv("DISPLAY", DISPLAY, 1);
        TRACE(stdout, "Display '%s' is now open.", DISPLAY);
    }
    /* Couldn't find an open display */
    else {
        TRACE(stdout, "An open display could not be found. Exiting.", "");
        exit(1);
    }
}



/* ************************ */
/* ***** SET OPEN TTY ***** */
/* ************************ */

/* Return an open TTY port */
/* * Find a better way to do this */
static void set_open_tty() {
    TRACE(stdout, "%s", "Searching for an open tty...");

    TTYN = 7;
    snprintf(VT, sizeof(VT), "%s%d", "vt", TTYN);

    TRACE(stdout, "TTY number is '%d'.", TTYN);
}
