/* *****************************************************************************
 * 
 * Name:    elyx.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "elyx.h"
 * 
 * Description: Setup the X server for the Elysia Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_ELYX_H
#define ELYSIA_ELYX_H

/* Defines */
#define X_CONFIG       "/etc/X11/elysia/src/login/config/Xsetup.config"
#define XCOMPMGR       "/usr/bin/xcompmgr"
#define XORG           "/usr/bin/Xorg"
#define XSERVER_AUTH   "/etc/X11/elysia/log/elysia.auth"
#define XSERVER_LOG    "/etc/X11/elysia/log/xserver.log"

/* Public functions */
void xsetup();

#endif /* ELYSIA_XSETUP_H */
