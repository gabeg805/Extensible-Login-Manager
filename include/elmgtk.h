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
#include <gtk/gtk.h>

/* Public functions */
int         elm_gtk_add_widget(GtkWidget **container, GtkWidget *widget);
int         elm_gtk_add_css_from_conf(GtkWidget **widget, char *selector,
                                      char *group, char *key);
int         elm_gtk_add_css_from_file(GtkWidget **widget, const char *selector,
                                      const char *file);
int         elm_gtk_default_widget(GtkWidget **window, GtkWidget **widget);
int         elm_gtk_focus(GtkWidget **widget);
int         elm_gtk_set_window_position(GtkWidget **window, unsigned int x,
                                        unsigned int y);
int         elm_gtk_set_window_size(GtkWidget **window, int width, int height);
int         elm_gtk_set_window_transparent(GtkWidget **window);
int         elm_gtk_set_widget_size(GtkWidget **widget, int width, int height);
int         elm_gtk_set_widget_size_from_conf(GtkWidget **widget,
                                              const char *group,
                                              const char *xkey,
                                              const char *ykey);
char *      elm_gtk_get_css_decl(char *name, char *args);
char *      elm_gtk_get_css_decl_bg(char *path);
char *      elm_gtk_get_css_rule(char *selector, char *declarations);
GtkWidget * elm_gtk_get_window(GtkWidget **widget);

#endif /* ELM_GTK_H */
