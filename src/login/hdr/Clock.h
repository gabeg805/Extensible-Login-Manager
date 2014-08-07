#include <gtk/gtk.h>
#include <time.h>

#ifndef CLOCK_H
#define CLOCK_H

void init_clock_root(GtkWidget *window, GtkWidget *clock);
void set_clock_color(GtkWidget *window, GtkWidget *clock);
void set_clock_label(GtkWidget *clock);
gboolean clock_update(gpointer data);


#endif
