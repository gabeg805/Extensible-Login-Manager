/* *****************************************************************************
 * 
 * Name:    xsession.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display username and password fields for ELM.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_XSESSION_H
#define ELM_XSESSION_H

/* Includes */
#include "elmapp.h"

/* Public functions */
GtkWidget *  new_xsession_widget(void);
GtkWidget *  get_xsession_button_widget(void);
GtkWidget *  get_xsession_menu_widget(void);
const char * get_xsession(void);

#endif /* ELM_XSESSION_H */
