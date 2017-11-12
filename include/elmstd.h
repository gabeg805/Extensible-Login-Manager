/* -----------------------------------------------------------------------------
 * 
 * Name:    elmstd.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Standard system functions.
 * 
 * Notes: None.
 * 
 * -----------------------------------------------------------------------------
 */

/* Header guard */
#ifndef ELM_STD_H
#define ELM_STD_H

/* Public functions  */
int  elm_std_execvp(char *file, char *const argv[]);
int  elm_std_setenv(char *name, char *value);
void elm_std_free(void *ptr);

#endif /* ELM_STD_H */
