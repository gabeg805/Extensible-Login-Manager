#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef PASSWORD_H
#define PASSWORD_H

void init_entry_root(GtkWidget *window, GtkWidget *entry);
void init_entry(GtkWidget *entry);
void set_entry_color(GtkWidget *window, GtkWidget *entry);
void get_entry_text(GtkWidget *entry);
char * password_entry(int argc, char *argv[]);

#endif
