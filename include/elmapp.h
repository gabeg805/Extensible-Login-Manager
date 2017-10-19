/* *****************************************************************************
 * 
 * Name:    elmapp.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: GTK+3 wrapper to set/get common attributes and rename functions
 *              more meaningful names.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_APP_H
#define ELM_APP_H

/* Includes */
#include "elmio.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

/* Callback function */
typedef void (*ElmCallback)(GtkWidget*, gpointer);

/* Position gravity */
typedef enum ElmGravity {
    ELM_GRAV_NONE=0,
    ELM_GRAV_TOP_LEFT,
    ELM_GRAV_TOP_RIGHT,
    ELM_GRAV_CENTER,
    ELM_GRAV_BOTTOM_LEFT,
    ELM_GRAV_BOTTOM_RIGHT
} ElmGravity;

/* Application structure */
typedef struct ElmApp
{
    GtkWidget *     (*display)(ElmCallback);
    enum ElmGravity   gravity;
    int16_t           x;
    int16_t           y;
} ElmApp;

#endif /* ELM_APP_H */
