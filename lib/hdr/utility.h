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
#include <stdio.h>
#include <stdbool.h>

/* Defines */
#define TRACE_LOC(stream) \
    fprintf(stream, "%s:%s:%d", __FILE__, __FUNCTION__, __LINE__)
#define TRACE_PRINT(stream, fmt, str) \
    fprintf(stream, fmt, str)

#define MAX_CMD_LEN    128
#define MAX_LOC_LEN    64
#define MAX_STR_LEN    32
#define MAX_NUM_LEN    16

/* Public functions  */
void usage();

void parse_argv(int argc, char **argv);

char * basename(char *str);
void get_substring(char *substr, char *str, char sep, int num);

void trace(FILE *handle);
void file_log(const char *fmt, ...);
void file_line_overwrite(char *file, char *key, char *val);

bool is_running(char *prog);
void get_cmd_output(char *arr, int size, char *cmd);

#endif /* ELYSIA_UTILITY_H */
