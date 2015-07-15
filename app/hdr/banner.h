/* *****************************************************************************
 * 
 * Name:    banner.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "banner.h"
 * 
 * Description: Display the login banner.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_BANNER_H
#define ELYSIA_BANNER_H

/* Defines */
#define BANNER_CONFIG    "/etc/X11/elysia/app/config/banner.cfg"
#define BANNER_STYLE     "/etc/X11/elysia/app/style/banner.css"

/* Public functions */
void display_banner();

#endif /* ELYSIA_BANNER_H */
