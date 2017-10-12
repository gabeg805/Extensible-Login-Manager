/* *****************************************************************************
 * 
 * Name:    credentials.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display username and password entries.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_CREDENTIALS_H
#define ELM_CREDENTIALS_H

/* Includes */
#include "elmapp.h"

/* Public functions */
GtkWidget *  new_username_widget(void);
GtkWidget *  new_password_widget(void);
GtkWidget *  get_username_widget(void);
GtkWidget *  get_password_widget(void);
const char * get_username(void);
const char * get_password(void);

#endif /* ELM_CREDENTIALS_H */
