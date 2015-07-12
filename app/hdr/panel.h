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
#define PANEL_SHUT_CONFIG    "/etc/X11/elysia/app/config/panel-shutdown.cfg"
#define PANEL_REF_CONFIG     "/etc/X11/elysia/app/config/panel-refresh.cfg"
#define PANEL_REB_CONFIG     "/etc/X11/elysia/app/config/panel-reboot.cfg"
#define PANEL_DIA_CONFIG     "/etc/X11/elysia/app/config/panel-dialog.cfg"
#define PANEL_SHUT_STYLE     "/etc/X11/elysia/app/style/panel-shutdown.css"
#define PANEL_REF_STYLE      "/etc/X11/elysia/app/style/panel-refresh.css"
#define PANEL_REB_STYLE      "/etc/X11/elysia/app/style/panel-reboot.css"
#define PANEL_DIA_STYLE      "/etc/X11/elysia/app/style/panel-dialog.css"
#define POWEROFF             "/usr/bin/poweroff"
#define REBOOT               "/usr/bin/reboot"
#define SYSTEMCTL            "/usr/bin/systemctl"

/* Public functions */
void display_panel();

#endif /* ELYSIA_PANEL_H */
