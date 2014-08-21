#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>

#ifndef TEXTIMAGE_H
#define TEXTIMAGE_H

void init_text_root(GtkWidget *window, GtkWidget *area);
void draw_text(cairo_t *);
gboolean draw_text_window(GtkWidget *window);
void display_text_image();

#endif
