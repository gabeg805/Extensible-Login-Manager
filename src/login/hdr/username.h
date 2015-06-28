/* *****************************************************************************
 * 
 * Name:    username.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "username.h"
 * 
 * Description: Display the username dropdown menu application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_USERNAME_H
#define ELYSIA_USERNAME_H

/* Defines */
#define USER_CONFIG        "/etc/X11/elysia/src/login/config/Username.config"
#define USER_IMG_CONFIG    "/etc/X11/elysia/src/login/config/Username-Image.config"

/* Public functions */
void display_username();

#endif /* ELYSIA_USERNAME_H */
