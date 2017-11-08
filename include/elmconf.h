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
char * elm_conf_read(const char *group, const char *key);
char * elm_conf_read_str(const char *group, const char *key);
int    elm_conf_read_int(const char *group, const char *key);
bool   elm_conf_read_bool(const char *group, const char *key);

#endif /* ELM_CONF_H */
