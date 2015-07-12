/* *****************************************************************************
 * 
 * Name:    clock.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "clock.h"
 * 
 * Description: Display the clock application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_CLOCK_H
#define ELYSIA_CLOCK_H

/* Defines */
#define DATE_CONFIG    "/etc/X11/elysia/app/config/date.cfg"
#define TIME_CONFIG    "/etc/X11/elysia/app/config/time.cfg"
#define DATE_STYLE     "/etc/X11/elysia/app/style/date.css"
#define TIME_STYLE     "/etc/X11/elysia/app/style/time.css"

/* Public functions */
void display_clock();

#endif /* ELYSIA_CLOCK_H */
