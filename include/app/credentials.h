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
#include "elmconf.h"
#include "elmgtk.h"
#include "elmsession.h"

/* Public functions */
GtkWidget * new_username_widget(void);
GtkWidget * new_password_widget(void);
void        set_credential_info(GtkWidget *widget, gpointer data);

#endif /* ELM_CREDENTIALS_H */
