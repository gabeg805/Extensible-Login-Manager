/* *****************************************************************************
 * 
 * Name:    elyapp.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Elysia Login Manager application builder.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elyapp.h"
#include "elytype.h"
#include "elyconfig.h"
#include "utility.h"
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>



/* **************************************
 * ***** SET APPLICATION ATTRIBUTES *****
 * **************************************
 */

/* Setup the application */
void set_app(struct elyapp *app)
{
    TRACE(stdout, "%s", "Setting up application...");

    /* Define widget settings from the config file */
    setup_config_pos(app);
    setup_config_shape(app);
    setup_config_text(app);
    set_app_pos(app);
    set_app_shape(app);

    /* Display the login frame */
    enable_transparency(app->gui.win);
    g_signal_connect(app->gui.win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_container_add(GTK_CONTAINER(app->gui.win), app->gui.widg);
    gtk_widget_show(app->gui.widg);
    gtk_widget_show(app->gui.win);

    TRACE(stdout, "%s", "Done setting up application");
}

/* Set widget position and size */
void set_app_pos(struct elyapp *app)
{
    TRACE(stdout, "%s", "Setting application position and size...");

    gtk_window_move(GTK_WINDOW(app->gui.win),
                    app->pos.x,
                    app->pos.y);

    TRACE(stdout, "%s", "Done setting position and size.");
}

/* Set widget position and size */
void set_app_shape(struct elyapp *app)
{
    TRACE(stdout, "%s", "Setting application position and size...");

    gtk_window_set_default_size(GTK_WINDOW(app->gui.win),
                                app->shape.width,
                                app->shape.height);

    TRACE(stdout, "%s", "Done setting position and size.");
}

/* Set the color of a widget  */
void set_app_style(struct elyapp *app, GtkWidget *widg)
{
    TRACE(stdout, "%s", "Setting application style...");

    /* No style defined */
    if ( access(app->settings.style, F_OK) == -1 )
        return;

    GtkStyleContext *context = gtk_widget_get_style_context(widg);
    GtkCssProvider *css      = gtk_css_provider_new();
    GFile *gfile             = g_file_new_for_path(app->settings.style);
    gtk_css_provider_load_from_file(css, gfile, NULL);
    gtk_style_context_add_class(context, app->settings.class);
    gtk_style_context_add_provider(context, 
                                   GTK_STYLE_PROVIDER(css), 
                                   GTK_STYLE_PROVIDER_PRIORITY_USER);

    TRACE(stdout, "%s", "Done setting color.");
}



/* ****************************
 * ***** SETUP APP FIELDS *****
 * ****************************
 */

/* Set application */
void setup_app(struct elyapp *app,
               struct elypos *pos,
               struct elyshape *shape,
               struct elytext *text,
               struct elysettings *settings)
{
    setup_app_pos(app, pos->x, pos->y);
    setup_app_shape(app, shape->width, shape->height, shape->curve, shape->img);
    setup_app_text(app, text->fmt, text->invis, text->maxchars);
    setup_app_settings(app, settings->config, settings->style, settings->class);
}

/* Set application settings files */
void setup_app_settings(struct elyapp *app,
                        char *config, char *style, char *class)
{
    app->settings.config = config;
    app->settings.style  = style;
    app->settings.class  = class;
}

/* Set application GTK GUI objects */
void setup_app_gui(struct elyapp *app,
                   GtkWidget *win, GtkWidget *widg)
{
    app->gui.win  = win;
    app->gui.widg = widg;
}

/* Set application position */
void setup_app_pos(struct elyapp *app,
                   int x, int y)
{
    app->pos.x = x;
    app->pos.y = y;
}

/* Set application shape dimensions */
void setup_app_shape(struct elyapp *app,
                     int width, int height, int curve, char *img)
{
    app->shape.width  = width;
    app->shape.height = height;
    app->shape.curve  = curve;
    app->shape.img    = img;
}

/* Set application textual attributes */
void setup_app_text(struct elyapp *app,
                    char *fmt, char *invis, int maxchars)
{
    app->text.fmt      = fmt;
    app->text.invis    = invis;
    app->text.maxchars = maxchars;
}



/* ****************************
 * ***** APP TRANSPARENCY *****
 * ****************************
 */

/* Enable widget transparency */
void enable_transparency(GtkWidget *win)
{
    TRACE(stdout, "%s", "Setting application transparency...");

    GdkScreen *screen = gtk_widget_get_screen(win);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    gtk_widget_set_visual(win, visual);

    TRACE(stdout, "%s", "Done setting transparency.");
}
