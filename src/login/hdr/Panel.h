#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>

#ifndef PANEL_H
#define PANEL_H

void init_shutdown_root(GtkWidget *shutdow_window, GtkWidget *shutdown);
void init_reboot_root(GtkWidget *reboot_window, GtkWidget *reboot);
void init_refresh_login_root(GtkWidget *refresh_window, GtkWidget *refresh);
void set_panel_color(GtkWidget *window, GtkWidget *panel);
void system_shutdown();
void system_reboot();
void refresh_login();

#endif
