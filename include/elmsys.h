/* *****************************************************************************
 * 
 * Name:    elmsys.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Common system functions.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_SYS_H
#define ELM_SYS_H

#include <unistd.h>

/* Public functions  */
int    elm_sys_exec(char *file, char *const argv[]);
int    elm_setenv(char *name, char *value);
pid_t  elm_sys_pgrep(const char *program);
char * elm_sys_basename(const char *string);
char * elm_sys_get_proc(void);
char * elm_sys_read_line(char *file);

#endif /* ELM_SYS_H */
