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
#include "elmio.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <gtk/gtk.h>

/* Globals */
/* ElmApp *APP = NULL; */
ElmApp *SELF = NULL;

/* Private functions */
static int elm_app_set_self(ElmApp *self);
static int elm_app_new_window_self(GtkWindowType type);
static int elm_app_new_window(ElmApp *self, GtkWindowType type);
static int elm_app_new_window_with_style_self(GtkWindowType type,
                                              const char *name,
                                              const char *file);
static int elm_app_new_window_with_style(ElmApp *self, GtkWindowType type,
                                         const char *name, const char *file);
static int elm_app_add_container_self(GtkWidget *container);
static int elm_app_add_container(ElmApp *self, GtkWidget *container);
static int elm_app_add_widget_self(GtkWidget *widget);
static int elm_app_add_widget(ElmApp *self, GtkWidget *widget);
static int elm_app_set_default_widget_self(GtkWidget *widget);
static int elm_app_set_default_widget(ElmApp *self, GtkWidget *widget);
static int elm_app_set_length_self(uint8_t length);
static int elm_app_set_length(ElmApp *self, uint8_t length);
static int elm_app_set_position_self(int16_t x, int16_t y);
static int elm_app_set_position(ElmApp *self, int16_t x, int16_t y);
static int elm_app_set_size_self(size_t width, size_t height);
static int elm_app_set_size(ElmApp *self, size_t width, size_t height);
static int elm_app_set_widget_size_self(GtkWidget *widget, size_t width,
                                        size_t height);
static int elm_app_set_widget_size(ElmApp *self, GtkWidget *widget,
                                   size_t width, size_t height);
static int elm_app_set_widget_style_self(const char *name, GtkWidget *widget,
                                         const char *file);
static int elm_app_set_widget_style(ElmApp *self, const char *name,
                                    GtkWidget *widget, const char *file);
static int elm_app_set_callback_self(ElmCallback callback);
static int elm_app_set_callback(ElmApp *self, ElmCallback callback);
static int elm_app_show_all_self(void);
static int elm_app_show_all(ElmApp *self);
static int elm_app_hide_all_self(void);
static int elm_app_hide_all(ElmApp *self);
static uint8_t elm_app_get_length_self(void);
static uint8_t elm_app_get_length(ElmApp *self);
static GtkWidget * elm_app_get_default_widget_self(void);
static GtkWidget * elm_app_get_default_widget(ElmApp *self);
static GtkWidget * elm_app_get_window_self(void);
static GtkWidget * elm_app_get_window(ElmApp *self);
static GtkWidget ** elm_app_get_widgets_self(void);
static GtkWidget ** elm_app_get_widgets(ElmApp *self);
static ElmCallback elm_app_get_callback_self(void);
static ElmCallback elm_app_get_callback(ElmApp *self);

static int elm_app_set_window_transparent(GtkWidget *window);

/* ************************************************************************** */
/* Construct a new ELM application */
ElmApp * elm_new_app(void)
{
    ElmApp *app = (ElmApp*)calloc(1, sizeof(ElmApp));
    app->set_self                  = &elm_app_set_self;
    app->new_window                = &elm_app_new_window_self;
    app->new_window_with_style     = &elm_app_new_window_with_style_self;
    app->add_container             = &elm_app_add_container_self;
    app->add_widget                = &elm_app_add_widget_self;
    app->set_default_widget        = &elm_app_set_default_widget_self;
    app->set_length                = &elm_app_set_length_self;
    app->set_position              = &elm_app_set_position_self;
    app->set_size                  = &elm_app_set_size_self;
    app->set_widget_size           = &elm_app_set_widget_size_self;
    app->set_widget_style          = &elm_app_set_widget_style_self;
    app->set_callback              = &elm_app_set_callback_self;
    app->show_all                  = &elm_app_show_all_self;
    app->hide_all                  = &elm_app_hide_all_self;
    app->get_default_widget        = &elm_app_get_default_widget_self;
    app->get_length                = &elm_app_get_length_self;
    app->get_widgets               = &elm_app_get_widgets_self;
    app->get_window                = &elm_app_get_window_self;
    app->get_callback              = &elm_app_get_callback_self;
    app->_window   = NULL;
    app->_widgets  = NULL;
    app->_callback = NULL;
    app->_length   = 0;
    SELF = app;
    return app;
}

/* ************************************************************************** */
/* Set self variable */
int elm_app_set_self(ElmApp *self)
{
    SELF = self;
    return 0;
}

/* ************************************************************************** */
/* Define window as local global window */
int elm_app_new_window_self(GtkWindowType type)
{
    return elm_app_new_window(SELF, type);
}

int elm_app_new_window(ElmApp *self, GtkWindowType type)
{
    self->_window      = gtk_window_new(type);
    GtkWidget *window = self->get_window();
    elm_app_set_window_transparent(window);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    return 0;
}

/* ************************************************************************** */
/* Define window as local global window and add a style */
int elm_app_new_window_with_style_self(GtkWindowType type, const char *name,
                                  const char *file)
{
    return elm_app_new_window_with_style(SELF, type, name, file);
}

int elm_app_new_window_with_style(ElmApp *self, GtkWindowType type,
                                  const char *name, const char *file)
{
    elm_app_new_window(self, type);
    return elm_app_set_widget_style(self, name, self->get_window(), file);
}

/* ************************************************************************** */
/* Add container to widget list and attach to window */
int elm_app_add_container_self(GtkWidget *container)
{
    return elm_app_add_container(SELF, container);
}

int elm_app_add_container(ElmApp *self, GtkWidget *container)
{
    GtkWidget *window = self->get_window();
    if ((window == NULL) || (container == NULL))
        return 1;
    int status;
    if ((status=self->add_widget(container)) != 0)
        return 1;
    gtk_container_add(GTK_CONTAINER(window), container);
    return 0;
}

/* ************************************************************************** */
/* Add widget to list */
int elm_app_add_widget_self(GtkWidget *widget)
{
    return elm_app_add_widget(SELF, widget);
}

int elm_app_add_widget(ElmApp *self, GtkWidget *widget)
{
    if (widget == NULL)
        return 1;
    uint8_t     length  = self->get_length();
    size_t      size    = sizeof(widget)*(length+1);
    self->_widgets       = (GtkWidget**)realloc(self->get_widgets(), size);
    GtkWidget **widgets = self->get_widgets();
    widgets[length] = widget;
    self->set_length(length+1);
    return 0;
}

/* ************************************************************************** */
/* Set default widget in window */
int elm_app_set_default_widget_self(GtkWidget *widget)
{
    return elm_app_set_default_widget(SELF, widget);
}

int elm_app_set_default_widget(ElmApp *self, GtkWidget *widget)
{
    GtkWidget *window = self->get_window();
    if ((widget == NULL) || (window == NULL))
        return 1;
    if (gtk_widget_has_default(widget))
        return 0;
    gtk_widget_set_can_default(widget, TRUE);
    gtk_window_set_default(GTK_WINDOW(window), widget);
    return 0;
}

/* ************************************************************************** */
/* Set length */
int elm_app_set_length_self(uint8_t length)
{
    return elm_app_set_length(SELF, length);
}

int elm_app_set_length(ElmApp *self, uint8_t length)
{
    self->_length = length;
    return 0;
}

/* ************************************************************************** */
/* Set window position */
int elm_app_set_position_self(int16_t x, int16_t y)
{
    return elm_app_set_position(SELF, x, y);
}

int elm_app_set_position(ElmApp *self, int16_t x, int16_t y)
{
    GtkWidget *window = self->get_window();
    if (window == NULL)
        return 1;
    gtk_window_move(GTK_WINDOW(window), x, y);
    return 0;
}

/* ************************************************************************** */
/* Set size of window */
int elm_app_set_size_self(size_t width, size_t height)
{
    return elm_app_set_size(SELF, width, height);
}

int elm_app_set_size(ElmApp *self, size_t width, size_t height)
{
    gtk_window_set_default_size(GTK_WINDOW(self->get_window()), width, height);
    return 0;
}


/* ************************************************************************** */
/* Set size for widget */
int elm_app_set_widget_size_self(GtkWidget *widget, size_t width,
                                 size_t height)
{
    return elm_app_set_widget_size(SELF, widget, width, height);
}

int elm_app_set_widget_size(ElmApp *self, GtkWidget *widget,
                            size_t width, size_t height)
{
    gtk_widget_set_size_request(widget, width, height);
    return 0;
}

/* ************************************************************************** */
/* Set style of widget */
int elm_app_set_widget_style_self(const char *name, GtkWidget *widget,
                                  const char *file)
{
    return elm_app_set_widget_style(SELF, name, widget, file);
}

int elm_app_set_widget_style(ElmApp *self, const char *name, GtkWidget *widget,
                             const char *file)
{
    if (access(file, F_OK) == -1)
        return 1;
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    GtkCssProvider  *css     = gtk_css_provider_new();
    GFile           *gfile   = g_file_new_for_path(file);
    gtk_css_provider_load_from_file(css, gfile, NULL);
    gtk_style_context_add_class(context, name);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(css), 
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);
    return 0;
}

/* ************************************************************************** */
/* Set callback */
int elm_app_set_callback_self(ElmCallback callback)
{
    return elm_app_set_callback(SELF, callback);
}

int elm_app_set_callback(ElmApp *self, ElmCallback callback)
{
    self->_callback = callback;
    return 0;
}

/* ************************************************************************** */
/* Show window and all widgets */
int elm_app_show_all_self(void)
{
    return elm_app_show_all(SELF);
}

int elm_app_show_all(ElmApp *self)
{
    GtkWidget *window = self->get_window();
    if (window == NULL)
        return 1;
    gtk_widget_show_all(window);
    return 0;
}

/* ************************************************************************** */
/* Hide window and all widgets */
int elm_app_hide_all_self(void)
{
    return elm_app_hide_all(SELF);
}

int elm_app_hide_all(ElmApp *self)
{
    GtkWidget *window = self->get_window();
    if (window == NULL)
        return 1;
    gtk_widget_hide(window);
    return 0;
}

/* ************************************************************************** */
/* Return number of widgets in application */
uint8_t elm_app_get_length_self(void)
{
    return elm_app_get_length(SELF);
}

uint8_t elm_app_get_length(ElmApp *self)
{
    return self->_length;
}

/* ************************************************************************** */
/* Return default widget in window */
GtkWidget * elm_app_get_default_widget_self(void)
{
    return elm_app_get_default_widget(SELF);
}

GtkWidget * elm_app_get_default_widget(ElmApp *self)
{
    GtkWidget *window = self->get_window();
    if (window == NULL)
        return NULL;
    return gtk_window_get_default_widget(GTK_WINDOW(window));
}

/* ************************************************************************** */
/* Return window widget */
GtkWidget * elm_app_get_window_self(void)
{
    return elm_app_get_window(SELF);
}

GtkWidget * elm_app_get_window(ElmApp *self)
{
    return self->_window;
}

/* ************************************************************************** */
/* Return widgets array */
GtkWidget ** elm_app_get_widgets_self(void)
{
    return elm_app_get_widgets(SELF);
}

GtkWidget ** elm_app_get_widgets(ElmApp *self)
{
    return self->_widgets;
}

/* ************************************************************************** */
/* Return callback */
ElmCallback elm_app_get_callback_self(void)
{
    return elm_app_get_callback(SELF);
}

ElmCallback elm_app_get_callback(ElmApp *self)
{
    return self->_callback;
}

/* ************************************************************************** */
/* ************************************************************************** */
/* UTILITIES */
/* ************************************************************************** */
/* ************************************************************************** */

/* ************************************************************************** */
/* Set window transparent */
int elm_app_set_window_transparent(GtkWidget *window)
{
    GdkScreen *screen = gtk_widget_get_screen(window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    gtk_widget_set_visual(window, visual);
    return 0;
}
