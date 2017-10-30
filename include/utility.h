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

#include <unistd.h>

/* Public functions  */
int    elm_sys_exec(char *file, char *const argv[]);
int    elm_setenv(char *name, char *value);
char * elm_sys_basename(const char *string);
pid_t  elm_sys_pgrep(const char *program);
char ** elm_sys_get_proc(void);

#endif /* ELM_UTILITY_H */
