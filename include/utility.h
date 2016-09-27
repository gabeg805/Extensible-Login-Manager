/* *****************************************************************************
 * 
 * Name:    utility.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
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
#include <unistd.h>

/* Maximum lengths for different strings */
#define MAX_CMD_LEN    128
#define MAX_LOC_LEN    64
#define MAX_STR_LEN    32
#define MAX_NUM_LEN    32

/* Public functions  */
void usage();
void parse_argv(int argc, char **argv);
char * basename(const char *string);
long pgrep(const char *program);
char * substring(const char *string, char *line);
void get_cmd_output(char *arr, int size, char *cmd);
pid_t exec_proc(char *cmd);

#endif /* ELYSIA_UTILITY_H */

// Change XDG variables
