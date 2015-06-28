/* *****************************************************************************
 * 
 * Name:    panel.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "panel.h"
 * 
 * Description: Display the button panel application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_PANEL_H
#define ELYSIA_PANEL_H

/* Defines */
#define PANEL_SHUT_CONFIG    "/etc/X11/elysia/src/login/config/Panel-Shutdown.config"
#define PANEL_REF_CONFIG     "/etc/X11/elysia/src/login/config/Panel-Refresh.config"
#define PANEL_REB_CONFIG     "/etc/X11/elysia/src/login/config/Panel-Reboot.config"
#define PANEL_DIA_CONFIG     "/etc/X11/elysia/src/login/config/Panel-Dialog.config"
#define POWEROFF             "/usr/bin/poweroff"
#define REBOOT               "/usr/bin/reboot"
#define SYSTEMCTL            "/usr/bin/systemctl"

/* Public functions */
void display_panel();

#endif /* ELYSIA_PANEL_H */
