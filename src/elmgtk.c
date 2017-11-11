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
#include "elmdef.h"
#include "elmsys.h"
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

/* ************************************************************************** */
/* Add widget to container */
int elm_gtk_add_widget(GtkWidget **container, GtkWidget *widget)
{
    if (!container || !widget) {
        return -1;
    }

    gtk_container_add(GTK_CONTAINER(*container), widget);

    return 0;
}

/* ************************************************************************** */
/* Add a CSS rule defined in a configuration file */
int elm_gtk_add_css_from_conf(GtkWidget **widget, char *group, char *key)
{
    /* Determine css rule */
    char *value = elm_conf_read(group, key);
    char *line  = elm_gtk_get_css_decl_bg(value);
    char *rule  = elm_gtk_get_css_rule(group, line);

    if (!value || !line || !rule) {
        return -1;
    }

    /* Load css rule */
    GtkStyleContext *context = gtk_widget_get_style_context(*widget);
    GtkCssProvider  *css     = gtk_css_provider_new();
    GError          *err     = NULL;

    gtk_css_provider_load_from_data(GTK_CSS_PROVIDER(css), rule, -1, &err);

    if (elm_is_key_err(&err)) {
        return -2;
    }

    /* Add rule to widget */
    gtk_style_context_add_class(context, group);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css);

    return 0;
}

/* ************************************************************************** */
/* Add CSS rules defined in a file */
int elm_gtk_add_css_from_file(GtkWidget **widget, const char *name, const char *file)
{
    if (access(file, F_OK)) {
        return -1;
    }

    /* Load css rule */
    GtkStyleContext *context = gtk_widget_get_style_context(*widget);
    GtkCssProvider  *css     = gtk_css_provider_new();
    GError          *err     = NULL;

    gtk_css_provider_load_from_path(css, file, &err);

    if (elm_is_key_err(&err)) {
        return -2;
    }

    /* Add rule to widget */
    if (name) {
        gtk_style_context_add_class(context, name);
    }

    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css), 
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css);

    return 0;
}

/* ************************************************************************** */
/* Set default widget in window */
int elm_gtk_default_widget(GtkWidget **window, GtkWidget **widget)
{
    if (gtk_widget_has_default(*widget)) {
        return 0;
    }

    gtk_widget_set_can_default(*widget, TRUE);
    gtk_window_set_default(GTK_WINDOW(*window), *widget);

    return 0;
}

/* ************************************************************************** */
/* Set focus on widget */
int elm_gtk_focus(GtkWidget **widget)
{
    gtk_widget_set_can_focus(*widget, TRUE);
    gtk_widget_grab_focus(*widget);

    return 0;
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
/* Set size of widget from config file */
int elm_gtk_set_widget_size_from_conf(GtkWidget **widget, const char *group,
                                      const char *xkey, const char *ykey)
{
    int width  = elm_conf_read_int(group, xkey);
    int height = elm_conf_read_int(group, ykey);

    if ((width < 0) || (height < 0)) {
        return -1;
    }

    return elm_gtk_set_widget_size(widget, width, height);
}

/* ************************************************************************** */
/* Return a declaration line for a CSS rule */
char * elm_gtk_get_css_decl(char *name, char *args)
{
    if (!name || !args) {
        return NULL;
    }

    char *line   = NULL;
    char *format = "  %s: %s;\n";
    char  buffer[ELM_MAX_LINE_SIZE];

    snprintf(buffer, sizeof(buffer), format, name, args);

    return elm_sys_strcpy(&line, buffer); /* can just make line NULL? */
}


/* ************************************************************************** */
/* Return a background image declaration line for a CSS rule */
char * elm_gtk_get_css_decl_bg(char *path)
{
    if (!path) {
        return NULL;
    }

    /* Determine css declaration */
    char *name   = "background-image";
    char  buffer[ELM_MAX_LINE_SIZE];

    snprintf(buffer, sizeof(buffer),
             "  %s: url('%s');\n  background-repeat: no-repeat;\n",
             name, path);

    return elm_sys_strcpy(NULL, buffer);
}

/* ************************************************************************** */
/* Return a CSS rule */
char * elm_gtk_get_css_rule(char *selector, char *declarations)
{
    if (!selector || !declarations) {
        return NULL;
    }

    /* Determine css rule */
    char *line = NULL;
    char  buffer[ELM_MAX_LINE_SIZE];

    snprintf(buffer, sizeof(buffer), ".%s {\n%s}\n", selector, declarations);

    return elm_sys_strcpy(&line, buffer);
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
