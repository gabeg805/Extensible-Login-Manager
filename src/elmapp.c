/* *****************************************************************************
 * 
 * Name:    elmapp.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Application builder, in the form of C-Style class, for the
 *              Extensible Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmapp.h"
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <gtk/gtk.h>

/* ************************************************************************** */
/* Set window position */
int elm_set_window_position(GtkWidget **window, int16_t x, int16_t y)
{
    gtk_window_move(GTK_WINDOW(*window), x, y);
    return 0;
}

/* ************************************************************************** */
/* Set size of window */
int elm_set_window_size(GtkWidget **window, size_t width, size_t height)
{
    gtk_window_set_default_size(GTK_WINDOW(*window), width, height);
    return 0;
}

/* ************************************************************************** */
/* Set window transparent */
int elm_set_window_transparent(GtkWidget **window)
{
    GdkScreen *screen = gtk_widget_get_screen(*window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    gtk_widget_set_visual(*window, visual);
    return 0;
}

/* ************************************************************************** */
/* Set default widget in window */
int elm_set_default_widget(GtkWidget **window, GtkWidget **widget)
{
    if (gtk_widget_has_default(*widget))
        return 0;
    gtk_widget_set_can_default(*widget, TRUE);
    gtk_window_set_default(GTK_WINDOW(*window), *widget);
    return 0;
}

/* ************************************************************************** */
/* Set size of widget */
int elm_set_widget_size(GtkWidget **widget, size_t width, size_t height)
{
    gtk_widget_set_size_request(*widget, width, height);
    return 0;
}

/* ************************************************************************** */
/* Set style of widget */
int elm_set_widget_style(GtkWidget **widget, const char *name, const char *file)
{
    if (access(file, F_OK) == -1)
        return 1;
    GtkStyleContext *context = gtk_widget_get_style_context(*widget);
    GtkCssProvider  *css     = gtk_css_provider_new();
    GFile           *gfile   = g_file_new_for_path(file);
    gtk_css_provider_load_from_file(css, gfile, NULL);
    gtk_style_context_add_class(context, name);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css), 
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    return 0;
}
