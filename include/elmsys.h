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

/* Includes */
#include <unistd.h>

/* Public functions  */
pid_t   elm_sys_pgrep(const char *program);
char *  elm_sys_basename(const char *string);
char ** elm_sys_get_proc(void);

#endif /* ELM_SYS_H */
