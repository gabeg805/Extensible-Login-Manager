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
#include "elysia.h"
#include "utility.h"
#include "benchmark.h"
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



/* ****************************** */
/* ***** SET OPEN X DISPLAY *****  */
/* ****************************** */

/* Return an open display (i.e. ':0') */
static void set_open_display() {
    double bmtime = benchmark_runtime(0);
    if ( VERBOSE )
        file_log("%s: (%s:%d): Searching for an open display...", 
                 __FILE__, __FUNCTION__, __LINE__);

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

        if ( VERBOSE )
            file_log("Done.\n%s: (%s:%d): Display '%s' is open.\n", 
                     __FILE__, __FUNCTION__, __LINE__, DISPLAY);
    }
    /* Couldn't find an open display */
    else {
        if ( VERBOSE )
            file_log("Error.\n%s (%s:%d): An open display could not be found. Exiting...\n", 
                     __FILE__, __FUNCTION__, __LINE__);
        exit(1);
    }

    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



/* ************************ */
/* ***** SET OPEN TTY ***** */
/* ************************ */

/* Return an open TTY port */
/* * Find a better way to do this */
static void set_open_tty() {
    double bmtime = benchmark_runtime(0);
    if ( VERBOSE )
        file_log("%s: (%s:%d): Determining TTY...", 
                 __FILE__, __FUNCTION__, __LINE__);

    TTYN = 7;
    snprintf(VT, sizeof(VT), "%s%d", "vt", TTYN);

    if ( VERBOSE )
        file_log("Done.\n%s: (%s:%d): TTY number is '%d'.\n", 
                 __FILE__, __FUNCTION__, __LINE__, TTYN);
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));

}



/* ************************** */
/* ***** START X SERVER ***** */
/* ************************** */

/* Start the X server */
static void start_xserver() {
    double bmtime = benchmark_runtime(0);
    if ( VERBOSE )
        file_log("%s: (%s:%d): Starting X server...\n", 
                 __FILE__, __FUNCTION__, __LINE__);

    /* Start X server */
    set_open_display();
    set_open_tty();
    pid_t child_pid = fork();
    if ( child_pid == 0 ) {
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
            file_log("%s: (%s:%d): Timeout reached: %s (%d sec). Elysia exited.",
                     __FILE__, __FUNCTION__, __LINE__, 
                     "X server timed out", timeout);
            exit(1);
        }
    }

    if ( VERBOSE )
        file_log("%s: (%s:%d): X server is active.\n", 
                 __FILE__, __FUNCTION__, __LINE__);
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



/* ************************************* */
/* ***** START COMPOSITING MANAGER ***** */
/* ************************************* */

/* Start compositing manager (for transparency) */
static void start_compman() {
    double bmtime = benchmark_runtime(0);
    if ( VERBOSE )
        file_log("%s: (%s:%d): Starting compositing manager...", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    /* Check if composite manager is already running */
    if ( is_running(basename(XCOMPMGR)) ) {
        if ( VERBOSE )
            file_log("Already running.\n");
        return;
    }

    /* Start compositing manager */
    pid_t pid = fork();
    if ( pid == 0 ) 
        execl(XCOMPMGR, XCOMPMGR, NULL);

    if ( VERBOSE )
        file_log("Done.\n");
    if ( BENCHTIME ) 
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



/* ******************* */
/* ***** SETUP X ***** */
/* ******************* */

/* Setup X for login manager */
void xsetup() {
    double bmtime = benchmark_runtime(0);

    if ( !PREVIEW ) 
        start_xserver();
    start_compman();
    exec_config_cmd(X_CONFIG, 1);
    exec_config_cmd(X_CONFIG, 2);
    INTERFACE = true;

    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}
