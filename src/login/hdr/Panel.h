#include <gtk/gtk.h>
#include <stdlib.h>

#ifndef PANEL_H
#define PANEL_H

void init_shutdown_root(GtkWidget *shutdow_window, GtkWidget *shutdown);
void init_reboot_root(GtkWidget *reboot_window, GtkWidget *reboot);
void init_glm_dialog_root(GtkWidget *refresh_window, GtkWidget *refresh);
void init_glm_dialog();
void system_shutdown();
void system_reboot();
void cancel_glm();
void refresh_glm();
void quit_glm();

#endif
