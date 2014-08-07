#include <gtk/gtk.h>

#ifndef TRANSPARENCY_H
#define TRANSPARENCY_H

void enable_transparency(GtkWidget *widget);

extern gboolean supports_alpha;

#endif
