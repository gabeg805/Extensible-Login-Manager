#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef USERNAME_H
#define USERNAME_H

void init_usermenu_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu, GtkWidget *label);
void init_userlabel(GtkWidget *label);
void usermenu_write_to_file(GtkMenu *item, GtkWidget *label);
char ** get_username(char *file, int size);
void set_username_entries(GtkWidget *menu, GtkWidget *label);

#endif
