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

/* Public functions  */
int elm_exec(char *file, char *const argv[]);
int elm_setenv(char *name, char *value);
char * basename(const char *string);
long pgrep(const char *program);
char * substring(const char *string, char *line);
void get_cmd_output(char *arr, int size, char *cmd);
pid_t exec_proc(char *cmd);

#endif /* ELM_UTILITY_H */
