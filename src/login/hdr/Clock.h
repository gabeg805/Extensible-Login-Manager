#include <gtk/gtk.h>
#include <time.h>

#ifndef CLOCK_H
#define CLOCK_H

void init_clock_date_root(GtkWidget *window, GtkWidget *clock);
void init_clock_time_root(GtkWidget *window, GtkWidget *clock);
void set_clock_label(GtkWidget *clock, char *fmt, const char *font, int fsize);
gboolean clock_date_update(gpointer data);
gboolean clock_time_update(gpointer data);
void display_clock();

#endif
