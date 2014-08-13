#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

void init_wm_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu);
void set_wm_color(GtkWidget *window, GtkWidget *dropmenu);
void wm_write_to_file(GtkMenu *item);
char * command_line(char *cmd);
void set_wm_entries(GtkWidget *menu);

#endif 
