/* *****************************************************************************
 * 
 * Name:    login.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display login fields for the Extensible Login Manager. This
 *              includes the title, username and password entry box, login
 *              button, and window manager menu.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_LOGIN_H
#define ELM_LOGIN_H

/* Includes */
#include "elmapp.h"
#include "elmgtk.h"

/* Public functions */
GtkWidget * display_login(ElmCallback callback);

#endif /* ELM_LOGIN_H */
