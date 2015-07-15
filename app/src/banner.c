/* *****************************************************************************
 * 
 * Name:    banner.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Display the login banner.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */
/* Includes */
#include "banner.h"
#include "elyapp.h"
#include "elyconfig.h"
#include "elyglobal.h"
#include "elytype.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

/* **********************************
 * ***** DISPLAY WINDOW MANAGER *****
 * **********************************
 */
/* Display the window manager button */
void display_banner()
{
    TRACE(stdout, "%s", "Displaying login banner...");

    /* Create the window manager menu application */
    static struct elyapp banner;
    banner.gui.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    banner.gui.widg = gtk_label_new("Login");
    setup_app_settings(&banner, BANNER_CONFIG, BANNER_STYLE, "Banner");
    set_app(&banner);
    set_app_style(&banner, banner.gui.win);
    set_app_style(&banner, banner.gui.widg);

    TRACE(stdout, "%s", "Done displaying banner.");
}
