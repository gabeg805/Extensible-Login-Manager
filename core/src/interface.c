/* *****************************************************************************
 * 
 * Name:    interface.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Display the Elysia Login Manager graphical user interface.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "interface.h"
#include "clock.h"
#include "frame.h"
#include "panel.h"
#include "password.h"
#include "textimage.h"
#include "username.h"
#include "utility.h"
#include "windowmanager.h"
#include <stdbool.h>
#include <gtk/gtk.h>



/* *********************************** */
/* ***** DISPLAY LOGIN INTERFACE *****  */
/* *********************************** */

/* Display the login interface */
void login_interface(void) {
    static bool interface = true;
    gtk_init(0, 0);

    if ( interface ) {
        TRACE(stdout, "%s", "Starting login interface...");
        display_frame();
        display_text_image();
        display_username();
        display_window_manager();
        display_clock();
        display_panel();
    }
    display_password_entry();

    if ( interface ) {
        TRACE(stdout, "%s", "Done displaying login interface.");
        interface = false;
    }

    gtk_main();
}
