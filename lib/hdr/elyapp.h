/* *****************************************************************************
 * 
 * Name:    elyapp.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "elyapp.h"
 * 
 * Description: Elysia Login Manager application builder.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_ELYAPP_H
#define ELYSIA_ELYAPP_H

/* Includes */
#include "elytype.h"
#include <gtk/gtk.h>

/* Public functions */
void set_app(struct elyapp *app);
void set_app_pos(struct elyapp *app);
void set_app_shape(struct elyapp *app);
void set_app_style(struct elyapp *app, GtkWidget *widg);

void setup_app(struct elyapp *app,
               struct elypos *pos,
               struct elyshape *shape,
               struct elytext *text,
               struct elysettings *settings);
void setup_app_settings(struct elyapp *app,
                        char *config, char *style, char *class);
void setup_app_gui(struct elyapp *app,
                   GtkWidget *win, GtkWidget *widg);
void setup_app_pos(struct elyapp *app,
                   int x, int y);
void setup_app_shape(struct elyapp *app,
                     int width, int height, int curve, char *img);
void setup_app_text(struct elyapp *app,
                    char *fmt, char *invis, int maxchars);

void enable_transparency(GtkWidget *win);

#endif /* ELYSIA_ELYAPP_H */
