/* *****************************************************************************
 * 
 * Name:    elmgtk.c
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

/* Includes */
#include "elmgtk.h"
#include "elmconf.h"
#include <stdlib.h>
#include <unistd.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

/* ************************************************************************** */
/* Add widget to container */
int elm_gtk_add(GtkWidget **container, GtkWidget *widget)
{
    if (!container || !widget) {
        return -1;
    }

    gtk_container_add(GTK_CONTAINER(*container), widget);

    return 0;
}

/* ************************************************************************** */
/* Show widget */
int elm_gtk_show(GtkWidget **widget)
{
    if (!widget) {
        return -1;
    }

    gtk_widget_show(*widget);

    return 0;
}

/* ************************************************************************** */
/* Show all widgets */
int elm_gtk_show_all(GtkWidget **widget)
{
    if (!widget) {
        return -1;
    }

    gtk_widget_show_all(*widget);

    return 0;
}

/* ************************************************************************** */
/* Allocate memory for new widget */
GtkWidget * elm_gtk_alloc(GtkWidget *widget, size_t size)
{
    GtkWidget *allocwidget = calloc(1, size);

    memcpy(allocwidget, widget, size);

    return allocwidget;
}

/* ************************************************************************** */
/* Set window position */
int elm_gtk_set_window_position(GtkWidget **window, unsigned int x, unsigned int y)
{
    gtk_window_move(GTK_WINDOW(*window), x, y);

    return 0;
}

/* ************************************************************************** */
/* Set size of window */
int elm_gtk_set_window_size(GtkWidget **window, int width, int height)
{
    if (!width || !height) {
        return -1;
    }

    gtk_window_set_default_size(GTK_WINDOW(*window), width, height);

    return 0;
}

/* ************************************************************************** */
/* Set window transparent */
int elm_gtk_set_window_transparent(GtkWidget **window)
{
    GdkScreen *screen = gtk_widget_get_screen(*window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);

    gtk_widget_set_visual(*window, visual);

    return 0;
}

/* ************************************************************************** */
/* Set default widget in window */
int elm_gtk_set_default_widget(GtkWidget **window, GtkWidget **widget)
{
    if (gtk_widget_has_default(*widget)) {
        return 0;
    }

    gtk_widget_set_can_default(*widget, TRUE);
    gtk_window_set_default(GTK_WINDOW(*window), *widget);

    return 0;
}

/* ************************************************************************** */
/* Set size of widget */
int elm_gtk_set_widget_size(GtkWidget **widget, int width, int height)
{
    if (!width || !height) {
        return -1;
    }

    gtk_widget_set_size_request(*widget, width, height);

    return 0;
}

/* ************************************************************************** */
/* Set style of widget */
int elm_gtk_set_widget_style(GtkWidget **widget, const char *name, const char *file)
{
    if (access(file, F_OK) == -1) {
        return 1;
    }

    GtkStyleContext *context = gtk_widget_get_style_context(*widget);
    GtkCssProvider  *css     = gtk_css_provider_new();
    GFile           *gfile   = g_file_new_for_path(file);

    gtk_css_provider_load_from_file(css, gfile, NULL);
    gtk_style_context_add_class(context, name);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css), 
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);

    return 0;
}

/* ************************************************************************** */
/* Return toplevel window */
GtkWidget * elm_gtk_get_window(GtkWidget **widget)
{
    GtkWidget *window = calloc(1, sizeof(GtkWindow));

    window = gtk_widget_get_toplevel(*widget);

    if (gtk_widget_is_toplevel(window)) {
        return window;
    }

    return NULL;
}

/* ************************************************************************** */
/* Return Gdk window */
GdkWindow * elm_gtk_get_gdkwindow(GtkWidget **widget)
{
    return gtk_widget_get_window(*widget);
}

/* ************************************************************************** */
/* Set size of widget from config file */
int elm_gtk_conf_set_widget_size(GtkWidget **widget, const char *group,
                                 const char *xkey, const char *ykey)
{
    int width  = elm_conf_read_int(group, xkey);
    int height = elm_conf_read_int(group, ykey);

    if ((width < 0) || (height < 0)) {
        return -1;
    }

    gtk_widget_set_size_request(*widget, width, height);

    return 0;
}
