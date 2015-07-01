/* *****************************************************************************
 * 
 * Name:    utility.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "utility.h"
 * 
 * Description: Common functions used by the Elysia Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_UTILITY_H
#define ELYSIA_UTILITY_H

/* Includes */
#include "elyglobal.h"
#include "benchmark.h"
#include <stdio.h>
#include <stdbool.h>

/* Defines */
/* Print to file pointer and stream */
#define FPRINTF(stream, fmt, str) \
    do { \
        fprintf(FP, fmt, str);     \
        fprintf(stream, fmt, str); \
    } while (0)

/* Enable debugging */
#ifdef DEBUG
#define TRACE_DEBUG(stream) \
    do { \
        FPRINTF(stream, "%s: ", __FILE__);    \
        FPRINTF(stream, "%s:", __FUNCTION__); \
        FPRINTF(stream, "%d: ", __LINE__);    \
    } while (0)
#else
#define TRACE_DEBUG(stream)
#endif

/* Enable benchmark testing */
#ifdef BENCHMARK
#define TRACE_BENCH(stream) \
    FPRINTF(stream, " (%.8f)", benchmark())
#else
#define TRACE_BENCH(stream)
#endif

/* Status print statement */
#define TRACE(stream, fmt, str) \
    do { \
        TRACE_DEBUG(stream);         \
        FPRINTF(stream, fmt, str);   \
        TRACE_BENCH(stream);         \
        FPRINTF(stream, "%s", "\n"); \
    } while (0)

/* Maximum lengths for different strings */
#define MAX_CMD_LEN    128
#define MAX_LOC_LEN    64
#define MAX_STR_LEN    32
#define MAX_NUM_LEN    16

/* Public functions  */
void usage();
void parse_argv(int argc, char **argv);

char * basename(char *str);
void get_substring(char *substr, char *str, char sep, int num);

bool is_running(char *prog);
void get_cmd_output(char *arr, int size, char *cmd);

#endif /* ELYSIA_UTILITY_H */
