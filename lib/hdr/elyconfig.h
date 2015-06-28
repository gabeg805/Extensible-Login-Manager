/* *****************************************************************************
 * 
 * Name:    elyconfig.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "elyconfig.h"
 * 
 * Description: Utilities for reading and parsing the configuration files.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_ELYCONFIG_H
#define ELYSIA_ELYCONFIG_H

/* Public functions */
char * strip_config_line(char *line, char sep);
char * read_config_char(char *file, char *key, int n);
int read_config_int(char *file, char *key);
void read_config_cmd_rep(char *arr, char *file, char *rep1, char *rep2, char *rep3);
void exec_config_cmd(char *file, int n);

#endif /* ELYSIA_ELYCONFIG_H */
