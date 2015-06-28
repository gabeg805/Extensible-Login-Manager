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
void setup_app(char *file, 
               struct elyapp *app, 
               char *event, 
               void (*func)(GtkWidget *widg));
void set_widget_pos(struct elyapp *app);
void set_widget_color(struct elyapp *app);
void enable_transparency(GtkWidget *widget);

void set_config_pos(char *file, struct elypos *pos);
void set_config_txt(char *file, struct elytxt *txt);
void set_config_decor(char *file, struct elydecor *decor);

#endif /* ELYSIA_ELYAPP_H */
