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

/* Typedefs */
typedef int (*ElmCallback)(GtkWidget*, void*);

typedef enum ElmGravity {
    ELM_GRAV_NONE=0,
    ELM_GRAV_TOP_LEFT,
    ELM_GRAV_TOP_RIGHT,
    ELM_GRAV_CENTER,
    ELM_GRAV_BOTTOM_LEFT,
    ELM_GRAV_BOTTOM_RIGHT
} ElmGravity;

typedef struct ElmApp
{
    GtkWidget *     (*display)(ElmCallback);
    enum ElmGravity   gravity;
    int16_t           x;
    int16_t           y;
} ElmApp;

typedef ElmApp * (*ElmAppBuilder)(ElmCallback);

/* Public functions */
int elm_set_window_position(GtkWidget **window, int16_t x, int16_t y);
int elm_set_window_size(GtkWidget **window, size_t width, size_t height);
int elm_set_window_transparent(GtkWidget **window);
int elm_set_default_widget(GtkWidget **window, GtkWidget **widget);
int elm_set_widget_size(GtkWidget **widget, size_t width, size_t height);
int elm_set_widget_style(GtkWidget **widget, const char *name,
                         const char *file);
GtkWidget * elm_get_window(GtkWidget **widget);
GdkWindow * elm_get_gdkwindow(GtkWidget **widget);

#endif /* ELM_APP_H */
