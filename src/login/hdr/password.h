/* *****************************************************************************
 * 
 * Name:    password.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "password.h"
 * 
 * Description: Display the password entry box application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_PASSWORD_H
#define ELYSIA_PASSWORD_H

/* Defines */
#define PASS_CONFIG    "/etc/X11/elysia/src/login/config/Password.config"

/* Public functions */
void display_password_entry();

#endif /* ELYSIA_PASSWORD_H */
