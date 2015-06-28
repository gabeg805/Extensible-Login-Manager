/* *****************************************************************************
 * 
 * Name:    authenticate.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "authenticate.h"
 * 
 * Description: Elysia Login Manager login authentication.
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
#define XINITRC        "/etc/X11/elysia/src/x/xinitrc"

/* Public functions */
int login(const char *username, const char *password);

#endif /* ELYSIA_AUTHENTICATE_H */
