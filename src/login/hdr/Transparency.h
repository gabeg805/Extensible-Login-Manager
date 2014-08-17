#include <gtk/gtk.h>

#ifndef TRANSPARENCY_H
#define TRANSPARENCY_H

void enable_transparency(GtkWidget *widget);
void set_color_and_opacity(GtkWidget *window, GtkWidget *widget, 
                           const GdkRGBA bg_widget, const GdkRGBA fg_widget);

gboolean supports_alpha;

#endif
