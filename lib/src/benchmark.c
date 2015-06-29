/* *****************************************************************************
 * 
 * Name:    benchmark.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Time the progress of program execution.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "benchmark.h"
#include <time.h>



/* **********************************
 * ***** TIME PROGRAM EXECUTION *****
 * **********************************
 */

double benchmark_runtime() {
    struct timespec t;
    /* clock_gettime(CLOCK_REALTIME, &t); */
    clock_gettime(CLOCK_MONOTONIC, &t);
    /* clock_gettime(CLOCK_MONOTONIC_COARSE, &t); */
    /* clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t); */
    /* clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t); */

    double time = (double)(t.tv_sec + (t.tv_nsec / 1e9));
    return time;
}
