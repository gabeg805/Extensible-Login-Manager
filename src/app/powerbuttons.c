/* *****************************************************************************
 * 
 * Name:    powerbuttons.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display power buttons that control shutting down and rebooting
 *              the system.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "app/powerbuttons.h"

/* Private functions */
static void system_prompt(GtkButton   *button, gpointer data);
static void system_shutdown(GtkButton *button, gpointer data);
static void system_reboot(GtkButton   *button, gpointer data);
static void system_cancel(GtkButton   *button, gpointer data);

/* Private variables */
static const char *Style    = "/etc/X11/elm/src/app/style/powerbuttons.css";
static const char *Shutdown = "/usr/bin/poweroff";
static const char *Reboot   = "/usr/bin/reboot";

/* ************************************************************************** */
/* Display system action options */
GtkWidget * display_power_buttons(ElmCallback callback)
{
    elmprintf(LOG, "Displaying power buttons.");

    static GtkWidget *button;
    button = gtk_button_new_from_icon_name("system-run", GTK_ICON_SIZE_LARGE_TOOLBAR);

    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    elm_set_widget_style(&button, "System", Style);
    g_signal_connect(button, "clicked", G_CALLBACK(system_prompt), NULL);

    return button;
}

/* ************************************************************************** */
/* Display system prompt popup window */
void system_prompt(GtkButton *button, gpointer data)
{
    size_t     margin    = 20;
    GtkWidget *dialog    = gtk_dialog_new();
    GtkWidget *shutdown  = gtk_button_new_with_label("Shutdown");
    GtkWidget *reboot    = gtk_button_new_with_label("Reboot");
    GtkWidget *cancel    = gtk_button_new_with_label("Cancel");
    GtkWidget *label     = gtk_label_new("Do you really want to shutdown?");
    GtkWidget *container = gtk_bin_get_child(GTK_BIN(dialog));
    GtkWidget *buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin);
    GtkWidget *systembox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin/2);
    GtkWidget *widget    = GTK_WIDGET(button);
    GtkWindow *window    = GTK_WINDOW(elm_get_window(&widget));

    gtk_box_pack_start(GTK_BOX(container), label,     TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(container), buttonbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttonbox), systembox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttonbox), cancel,    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(systembox), shutdown,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(systembox), reboot,    FALSE, FALSE, 0);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_margin_top(container,    margin/2);
    gtk_widget_set_margin_bottom(container, margin/2);
    gtk_widget_set_margin_start(container,  margin/2);
    gtk_widget_set_margin_end(container,    margin/2);
    gtk_widget_set_margin_bottom(label,     margin);
    elm_set_widget_style(&shutdown, "Shutdown", Style);
    g_signal_connect(shutdown, "clicked", G_CALLBACK(system_shutdown), &dialog);
    g_signal_connect(reboot,   "clicked", G_CALLBACK(system_reboot),   &dialog);
    g_signal_connect(cancel,   "clicked", G_CALLBACK(system_cancel),   &dialog);

    gtk_dialog_set_default_response(GTK_DIALOG(dialog), 0);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

    gtk_widget_grab_focus(cancel);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* ************************************************************************** */
/* Send shutdown response */
void system_shutdown(GtkButton *button, gpointer data)
{
    elmprintf(LOG, "Shutting down.");
    execl(Shutdown, Shutdown, NULL);
}

/* ************************************************************************** */
/* Send reboot response */
void system_reboot(GtkButton *button, gpointer data)
{
    elmprintf(LOG, "Rebooting.");
    execl(Reboot, Reboot, NULL);
}

/* ************************************************************************** */
/* Send cancel response */
void system_cancel(GtkButton *button, gpointer data)
{
    elmprintf(LOG, "Cancel.");
    GtkWidget **dialog = data;
    gtk_dialog_response(GTK_DIALOG(*dialog), 0);
}
