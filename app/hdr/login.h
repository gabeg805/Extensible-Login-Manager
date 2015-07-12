/* *****************************************************************************
 * 
 * Name:    login.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "login.h"
 * 
 * Description: Display the username and password entry box applications.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_LOGIN_H
#define ELYSIA_LOGIN_H

/* Defines */
#define USER_CONFIG      "/etc/X11/elysia/app/config/username.cfg"
#define PASS_CONFIG      "/etc/X11/elysia/app/config/password.cfg"
#define SIGNIN_CONFIG    "/etc/X11/elysia/app/config/signin.cfg"
#define USER_STYLE       "/etc/X11/elysia/app/style/username.css"
#define PASS_STYLE       "/etc/X11/elysia/app/style/password.css"
#define SIGNIN_STYLE     "/etc/X11/elysia/app/style/signin.css"

/* Public functions */
void display_login();

#endif /* ELYSIA_PASSWORD_H */
