/* *****************************************************************************
 * 
 * Name:    powerbuttons.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display power buttons that control shutting down and rebooting
 *              the system.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_POWER_BUTTONS_H
#define ELM_POWER_BUTTONS_H

/* Includes */
#include "elmapp.h"
#include "elmgtk.h"
#include "elmconf.h"

/* Public functions */
GtkWidget * display_power_buttons(ElmCallback callback);

#endif /* ELM_POWER_BUTTONS_H */
