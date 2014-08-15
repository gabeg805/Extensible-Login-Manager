#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

void init_wm_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu);
void wm_write_to_file(GtkMenu *item);
void set_wm_entries(GtkWidget *menu);

#endif 
