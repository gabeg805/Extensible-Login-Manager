/* *****************************************************************************
 * 
 * Name:    elmconf.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Parse config files.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_CONF_H
#define ELM_CONF_H

/* Includes */
#include <stdbool.h>

/* Public functions */
char * elm_conf_read(char *group, char *key);
char * elm_conf_read_str(char *group, char *key);
int    elm_conf_read_int(char *group, char *key);
bool   elm_conf_read_bool(char *group, char *key);

#endif /* ELM_CONF_H */
