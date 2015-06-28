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
#define CLOCK_DATE_CONFIG    "/etc/X11/elysia/app/config/clock-date.cfg"
#define CLOCK_TIME_CONFIG    "/etc/X11/elysia/app/config/clock-time.cfg"

/* Public functions */
void display_clock();

#endif /* ELYSIA_CLOCK_H */
