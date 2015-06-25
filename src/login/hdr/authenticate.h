/* *****************************************************************************
 * 
 * Name:    authenticate.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "authenticate.h"
 * 
 * Description: Login to the system.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_AUTHENTICATE_H
#define ELYSIA_AUTHENTICATE_H

/* Defines */
#define AUTH_CONFIG    "/etc/X11/elysia/src/login/config/Authenticate.config"
#define SESSREG        "/usr/bin/sessreg"
#define UTMP_ADD       "/run/utmp"
#define UTMP_DEL       "/var/run/utmp"
#define WTMP           "/var/log/wtmp"
#define XINITRC        "/etc/X11/elysia/src/x/xinitrc"

/* Public functions */
int login(const char *username, const char *password);

#endif /* ELYSIA_AUTHENTICATE_H */
