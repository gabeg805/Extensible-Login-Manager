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
#include "elmio.h"

/* Private functions */
static void system_shutdown(void);
static void system_reboot(void);

/* Private variables */
static const char *Style        = "/etc/X11/elm/src/app/style/powerbuttons.css";
static const char *RebootFile   = "/etc/X11/elm/img/reboot.png";
static const char *ShutdownFile = "/etc/X11/elm/img/shutdown.png";

/* ************************************************************************** */
/* Create power button application */
GtkWidget * display_power_buttons(ElmCallback callback)
{
    elmprintf(LOG, "Displaying power buttons...");

    GtkWidget        *rbimg = gtk_image_new_from_file(RebootFile);
    GtkWidget        *sdimg = gtk_image_new_from_file(ShutdownFile);
    static GtkWidget *box;
    static GtkWidget *reboot;
    static GtkWidget *shutdown;
    box      = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    reboot   = gtk_button_new();
    shutdown = gtk_button_new();

    gtk_button_set_image(GTK_BUTTON(reboot),   rbimg);
    gtk_button_set_image(GTK_BUTTON(shutdown), sdimg);
    gtk_button_set_relief(GTK_BUTTON(reboot),   GTK_RELIEF_NONE);
    gtk_button_set_relief(GTK_BUTTON(shutdown), GTK_RELIEF_NONE);
    gtk_box_pack_start(GTK_BOX(box), reboot,  FALSE,  FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), shutdown, FALSE, FALSE, 0);
    elm_set_widget_style(&reboot,   "Reboot",   Style);
    elm_set_widget_style(&shutdown, "Shutdown", Style);
    g_signal_connect(reboot,   "clicked", G_CALLBACK(system_reboot),   NULL);
    g_signal_connect(shutdown, "clicked", G_CALLBACK(system_shutdown), NULL);

    return box;
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
