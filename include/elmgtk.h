/* *****************************************************************************
 * 
 * Name:    elmgtk.h
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
#ifndef ELM_GTK_H
#define ELM_GTK_H

/* Includes */
#include "elmio.h"
#include <stddef.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

/* Public functions */
int         elm_gtk_add(GtkWidget **container, GtkWidget *widget);
int         elm_gtk_show(GtkWidget **widget);
int         elm_gtk_show_all(GtkWidget **widget);
GtkWidget * elm_gtk_alloc(GtkWidget *widget, size_t size);
int         elm_gtk_set_window_position(GtkWidget **window, unsigned int x, unsigned int y);
int         elm_gtk_set_window_size(GtkWidget **window, int width, int height);
int         elm_gtk_set_window_transparent(GtkWidget **window);
int         elm_gtk_set_default_widget(GtkWidget **window, GtkWidget **widget);
int         elm_gtk_set_widget_size(GtkWidget **widget, int width, int height);
int         elm_gtk_set_widget_style(GtkWidget **widget, const char *name,
                             const char *file);
GtkWidget * elm_gtk_get_window(GtkWidget **widget);
GdkWindow * elm_gtk_get_gdkwindow(GtkWidget **widget);

#endif /* ELM_GTK_H */
