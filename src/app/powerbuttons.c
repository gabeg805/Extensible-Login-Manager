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
static void elm_app_system_prompt(GtkButton   *button, gpointer data);
static void elm_app_system_shutdown(GtkButton *button, gpointer data);
static void elm_app_system_reboot(GtkButton   *button, gpointer data);
static void elm_app_system_cancel(GtkButton   *button, gpointer data);

/* Private variables */
static const char *Style = "/etc/X11/elm/share/css/powerbuttons.css";

/* ************************************************************************** */
/* Display system action options */
GtkWidget * display_power_buttons(ElmCallback callback)
{
    elmprintf(LOGINFO, "Displaying system power button.");

    /* Create widget */
    static GtkWidget *button = NULL;

    button = gtk_button_new();

    /* Setup widget */
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    elm_gtk_add_css_from_conf(&button, "PowerButton", "Images", "Power");
    elm_gtk_add_css_from_file(&button, "PowerButton", Style);
    elm_gtk_set_widget_size_from_conf(&button, "Powerbuttons", "Width", "Height");

    g_signal_connect(button, "clicked", G_CALLBACK(elm_app_system_prompt), NULL);
    gtk_widget_show(button);

    return button;
}

/* ************************************************************************** */
/* Display system prompt popup window */
void elm_app_system_prompt(GtkButton *button, gpointer data)
{
    /* Create dialog prompt */
    size_t     margin    = 20;
    GtkWidget *dialog    = gtk_dialog_new();
    GtkWidget *shutdown  = gtk_button_new_with_label("Shutdown");
    GtkWidget *reboot    = gtk_button_new_with_label("Reboot");
    GtkWidget *cancel    = gtk_button_new_with_label("Cancel");
    GtkWidget *label     = gtk_label_new("Do you really want to shutdown the computer?");
    GtkWidget *container = gtk_bin_get_child(GTK_BIN(dialog));
    GtkWidget *buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin*2);
    GtkWidget *systembox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, margin/2);
    GtkWidget *widget    = GTK_WIDGET(button);
    GtkWindow *window    = GTK_WINDOW(elm_gtk_get_window(&widget));

    /* Setup dialog prompt */
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

    elm_gtk_add_css_from_file(&shutdown, "ShutdownButton", Style);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), 0);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), window);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

    g_signal_connect(shutdown, "clicked", G_CALLBACK(elm_app_system_shutdown), &dialog);
    g_signal_connect(reboot,   "clicked", G_CALLBACK(elm_app_system_reboot),   &dialog);
    g_signal_connect(cancel,   "clicked", G_CALLBACK(elm_app_system_cancel),   &dialog);

    /* Display dialog prompt */
    elm_gtk_focus(&cancel);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

/* ************************************************************************** */
/* Send shutdown response */
void elm_app_system_shutdown(GtkButton *button, gpointer data)
{
    elmprintf(LOGINFO, "Shutting down.");
    execl(ELM_CMD_SHUTDOWN, ELM_CMD_SHUTDOWN, NULL);
}

/* ************************************************************************** */
/* Send reboot response */
void elm_app_system_reboot(GtkButton *button, gpointer data)
{
    elmprintf(LOGINFO, "Rebooting.");
    execl(ELM_CMD_REBOOT, ELM_CMD_REBOOT, NULL);
}

/* ************************************************************************** */
/* Send cancel response */
void elm_app_system_cancel(GtkButton *button, gpointer data)
{
    elmprintf(LOGINFO, "Cancel.");
    GtkWidget **dialog = data;
    gtk_dialog_response(GTK_DIALOG(*dialog), 0);
}
