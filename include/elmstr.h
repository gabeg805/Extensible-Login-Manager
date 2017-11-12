/* -----------------------------------------------------------------------------
 * 
 * Name:    elmstr.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Common strtem functions.
 * 
 * Notes: None.
 * 
 * -----------------------------------------------------------------------------
 */

/* Header guard */
#ifndef ELM_STR_H
#define ELM_STR_H

/* Includes */
#include <stdlib.h>

/* Public functions  */
char * elm_str_copy(char *string);
char * elm_str_vcopy(size_t size, const char *format, ...);
char * elm_str_path(const char *format, ...);
char * elm_str_readline(char *file);
char * elm_str_findline(char *file, char *substring);
char * elm_str_strip(char *string);

#endif /* ELM_STR_H */
