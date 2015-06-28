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
#include "benchmark.h"
#include <gtk/gtk.h>



/* ***************************** */
/* ***** APPLICATION SETUP ***** */
/* ***************************** */

/* Setup the widget */
void setup_app(char *file, 
               struct elyapp *app, 
               char *event, 
               void (*func)(GtkWidget *widg))
{
    double bmtime = benchmark_runtime(0);

    /* Define widget settings from the config file */
    set_config_pos(file,   &app->pos);
    set_config_txt(file,   &app->txt);
    set_config_decor(file, &app->decor);
    set_widget_pos(app);
    set_widget_color(app);

    /* Set widget settings */
    enable_transparency(app->win);
    gtk_container_add(GTK_CONTAINER(app->win), app->widg);
    g_signal_connect(app->win, "destroy", G_CALLBACK(gtk_main_quit), 0);
    if ( event != 0 )
        g_signal_connect(G_OBJECT(app->widg), event, G_CALLBACK(func), 0);

    /* Display the login frame */
    gtk_widget_show(app->widg);
    gtk_widget_show(app->win);

    /* if ( BENCHTIME ) */
    /*     file_log("%s: (%s: Runtime): %lf\n",  */
    /*              __FILE__, __FUNCTION__, benchmark_runtime(bmtime)); */
}



/* Set widget position and size */
void set_widget_pos(struct elyapp *app)
{
    gtk_window_move(GTK_WINDOW(app->win),
                    app->pos.x,
                    app->pos.y);
    gtk_window_set_default_size(GTK_WINDOW(app->win),
                                app->pos.width,
                                app->pos.height);
}



/* Set the color of a widget  */
void set_widget_color(struct elyapp *app)
{
    /* No decoration defined */
    if ( app->decor.div < 0 )
        return;

    /* Define widget/window dackground and foreground colors */
    double bg_red         = (double) app->decor.bg_red   / app->decor.div;
    double bg_green       = (double) app->decor.bg_green / app->decor.div;
    double bg_blue        = (double) app->decor.bg_blue  / app->decor.div;
    double bg_alpha       = (double) app->decor.bg_alpha / app->decor.div;
    double fg_red         = (double) app->decor.fg_red   / app->decor.div;
    double fg_green       = (double) app->decor.fg_green / app->decor.div;
    double fg_blue        = (double) app->decor.fg_blue  / app->decor.div;
    double fg_alpha       = (double) app->decor.fg_alpha / app->decor.div;
    const GdkRGBA bg_win  = {0, 0, 0, 0};
    const GdkRGBA fg_win  = {0, 0, 0, 0};
    const GdkRGBA bg_widg = {bg_red, bg_green, bg_blue, bg_alpha};
    const GdkRGBA fg_widg = {fg_red, fg_green, fg_blue, fg_alpha};

    /* Set widget/window colors */
    gtk_widget_override_background_color(app->win, GTK_STATE_FLAG_NORMAL, &bg_win);
    gtk_widget_override_background_color(app->widg, GTK_STATE_FLAG_NORMAL, &bg_widg);
    gtk_widget_override_color(app->win, GTK_STATE_FLAG_NORMAL, &fg_win);
    gtk_widget_override_color(app->widg, GTK_STATE_FLAG_NORMAL, &fg_widg);
}



/* Enable widget transparency */
void enable_transparency(GtkWidget *widg)
{
    GdkScreen *screen = gtk_widget_get_screen(widg);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    gtk_widget_set_visual(widg, visual);
}



/* ***************************** */
/* ***** SET CONFIG VALUES ***** */
/* ***************************** */

/* Set position values from config file */
void set_config_pos(char *file, struct elypos *pos)
{
    pos->x      = read_config_int(file, "xpos");
    pos->y      = read_config_int(file, "ypos");
    pos->width  = read_config_int(file, "width");
    pos->height = read_config_int(file, "height");

    /* Set default values when no values in config file are set */
    if ( pos->x < 0 )      { pos->x = 0; }
    if ( pos->y < 0 )      { pos->y = 0; }
    if ( pos->width < 0 )  { pos->width = 0; }
    if ( pos->height < 0 ) { pos->height = 0; }
}



/* Set text values from config file */
void set_config_txt(char *file, struct elytxt *txt)
{
    txt->size     = read_config_int(file,  "size");
    txt->maxchars = read_config_int(file,  "maxchars");
    txt->refresh  = read_config_int(file,  "refresh-sec");
    txt->text     = read_config_char(file, "text",  MAX_STR_LEN);
    txt->font     = read_config_char(file, "font",  MAX_STR_LEN);
    txt->fmt      = read_config_char(file, "fmt",   MAX_STR_LEN);
    txt->invis    = read_config_char(file, "invis", MAX_STR_LEN);
    txt->red      = read_config_int(file,  "txt-red");
    txt->green    = read_config_int(file,  "txt-green");
    txt->blue     = read_config_int(file,  "txt-blue");

    /* Set default values when no value in config file is set */
    if ( txt->invis == 0 )
        txt->invis = " ";
}



/* Set decoration values from config file */
void set_config_decor(char *file, struct elydecor *decor)
{
    decor->img_file = read_config_char(file, "img-file", MAX_LOC_LEN);
    decor->bg_red   = read_config_int(file, "bg-red");
    decor->bg_green = read_config_int(file, "bg-green");
    decor->bg_blue  = read_config_int(file, "bg-blue");
    decor->bg_alpha = read_config_int(file, "bg-alpha");
    decor->fg_red   = read_config_int(file, "fg-red");
    decor->fg_green = read_config_int(file, "fg-green");
    decor->fg_blue  = read_config_int(file, "fg-blue");
    decor->fg_alpha = read_config_int(file, "fg-alpha");
    decor->div      = read_config_int(file, "div");
}
