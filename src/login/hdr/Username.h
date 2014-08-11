#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef USERNAME_H
#define USERNAME_H

void init_usermenu_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu, GtkWidget *label);
void init_userlabel(GtkWidget *label);
void set_usermenu_color(GtkWidget *window, GtkWidget *dropmenu);
void usermenu_write_to_file(GtkMenu *item, GtkWidget *label);
void set_username_entries(GtkWidget *menu, GtkWidget *label);

#endif
