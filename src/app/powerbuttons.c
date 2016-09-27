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
#include "elm.h"

/* Private functions */
static int set_widget_container(ElmApp *power);
static int set_reboot_button(ElmApp *power);
static int set_shutdown_button(ElmApp *power);
static int set_power_button_settings(GtkWidget *widget, const char *file,
                                     void (*callback)(void));
static void system_shutdown(void);
static void system_reboot(void);

/* Private variables */
static const char *Style = "/etc/X11/elm/src/app/style/powerbuttons.css";

/* ************************************************************************** */
/* Create power button application */
ElmApp * display_power_buttons(ElmCallback callback)
{
    elmprintf(LOG, "Displaying power buttons...");
    static ElmApp *power;
    power = elm_new_app();
    power->new_window_with_style(GTK_WINDOW_TOPLEVEL, "Window", Style);
    power->set_position(1270, 725);
    set_widget_container(power);
    set_reboot_button(power);
    set_shutdown_button(power);
    elmprintf(LOG, "Done displaying power buttons...");
    return power;
}

/* ************************************************************************** */
/* Create power button container */
int set_widget_container(ElmApp *power)
{
    static GtkWidget *container;
    container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    power->set_self(power);
    power->add_container(container);
    return 0;
}

/* ************************************************************************** */
/* Create reboot button */
int set_reboot_button(ElmApp *power)
{
    const  char      *file = "/etc/X11/elm/img/reboot.png";
    static GtkWidget *reboot;
    reboot = gtk_button_new();
    set_power_button_settings(reboot, file, system_reboot);
    GtkWidget *container = power->get_widgets()[0];
    gtk_box_pack_start(GTK_BOX(container), reboot, FALSE, FALSE, 0);
    power->set_self(power);
    power->add_widget(reboot);
    power->set_widget_style("Reboot", reboot, Style);
    return 0;
}

/* ************************************************************************** */
/* Create shutdown button */
int set_shutdown_button(ElmApp *power)
{
    const  char      *file = "/etc/X11/elm/img/shutdown.png";
    static GtkWidget *shutdown;
    shutdown = gtk_button_new();
    set_power_button_settings(shutdown, file, system_shutdown);
    GtkWidget *container = power->get_widgets()[0];
    gtk_box_pack_start(GTK_BOX(container), shutdown, FALSE, FALSE, 0);
    power->set_self(power);
    power->add_widget(shutdown);
    power->set_widget_style("Shutdown", shutdown, Style);
    return 0;
}

/* ************************************************************************** */
/* Set settings for a power button */
int set_power_button_settings(GtkWidget *widget, const char *file,
                              void (*callback)(void))
{
    GtkWidget *image = gtk_image_new_from_file(file);
    gtk_button_set_image(GTK_BUTTON(widget), image);
    gtk_button_set_relief(GTK_BUTTON(widget), GTK_RELIEF_NONE);
    g_signal_connect(widget, "clicked", G_CALLBACK(callback), NULL);
    return 0;
}

/* ************************************************************************** */
/* Shutdown */
void system_shutdown(void)
{
    elmprintf(LOG, "Shutting down...");
    const char *poweroff = "/usr/bin/poweroff";
    execl(poweroff, poweroff, NULL);
}

/* ************************************************************************** */
/* Reboot */
void system_reboot(void)
{
    elmprintf(LOG, "Rebooting...");
    const char *reboot = "/usr/bin/reboot";
    execl(reboot, reboot, NULL);
}
