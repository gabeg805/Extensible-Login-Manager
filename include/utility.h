/* *****************************************************************************
 * 
 * Name:    utility.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Common functions used by ELM.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_UTILITY_H
#define ELM_UTILITY_H

/* Includes */
#include "benchmark.h"
#include <unistd.h>

/* Maximum lengths for different strings */
#define MAX_CMD_LEN    128
#define MAX_LOC_LEN    64
#define MAX_STR_LEN    32
#define MAX_NUM_LEN    32

/* Public functions  */
char * basename(const char *string);
long pgrep(const char *program);
char * substring(const char *string, char *line);
void get_cmd_output(char *arr, int size, char *cmd);
pid_t exec_proc(char *cmd);

#endif /* ELM_UTILITY_H */
