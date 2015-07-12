/* *****************************************************************************
 * 
 * Name:    windowmanager.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "windowmanager.h"
 * 
 * Description: Display the window manager button application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_WINDOWMANAGER_H
#define ELYSIA_WINDOWMANAGER_H

/* Defines */
#define WM_CONFIG    "/etc/X11/elysia/app/config/windowmanager.cfg"
#define WM_STYLE     "/etc/X11/elysia/app/style/windowmanager.css"

/* Public functions */
void display_window_manager();

#endif /* ELYSIA_WINDOWMANAGER_H */
