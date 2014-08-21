#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>

#ifndef FRAME_H
#define FRAME_H

void init_frame_root(GtkWidget *window, GtkWidget *area);
void draw_frame(cairo_t *);
gboolean draw_frame_window(GtkWidget *widget);
void display_frame();

#endif
