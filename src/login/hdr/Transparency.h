#include <gtk/gtk.h>
#include "../hdr/Config.h"

#ifndef TRANSPARENCY_H
#define TRANSPARENCY_H

void enable_transparency(GtkWidget *widget);
void set_color_and_opacity(GtkWidget *window, GtkWidget *widget, 
                           const GdkRGBA bg_widget, const GdkRGBA fg_widget);
void set_stuff(struct glmstruct *gui);
void set_ass(GtkWidget *win, GtkWidget *widg, const GdkRGBA color[4]);

gboolean supports_alpha;

#endif
