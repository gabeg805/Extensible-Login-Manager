#include <gtk/gtk.h>
#include <gdk/gdk.h>

#ifndef POWER_H
#define POWER_H

void init_shutdown_root(GtkWidget *shutdow_window, GtkWidget *shutdown);
void init_reboot_root(GtkWidget *reboot_window, GtkWidget *reboot);
void set_power_color(GtkWidget *window, GtkWidget *power);
void system_shutdown();
void system_reboot();

#endif
