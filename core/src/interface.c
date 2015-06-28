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
#include "elysia.h"
#include "username.h"
#include "password.h"
#include "clock.h"
#include "frame.h"
#include "textimage.h"
#include "windowmanager.h"
#include "panel.h"
#include "utility.h"
#include "benchmark.h"
#include <stdbool.h>
#include <gtk/gtk.h>



/* *********************************** */
/* ***** DISPLAY LOGIN INTERFACE *****  */
/* *********************************** */

/* Display the login interface */
void login_interface(void) {
    double bmtime = benchmark_runtime(0);

    gtk_init(0, 0);

    if ( INTERFACE ) {
        if ( VERBOSE )
            file_log("%s: (%s:%d): Displaying login interface...\n", 
                     __FILE__, __FUNCTION__, __LINE__);

        /* Display interface items */
        display_frame();
        display_text_image();
        display_username();
        display_window_manager();
        display_clock();
        display_panel();

        INTERFACE = false;
    }
    display_password_entry();

    if ( VERBOSE )
        file_log("%s: (%s:%d): Done displaying login interface.\n",
                 __FILE__, __FUNCTION__, __LINE__);
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));

    gtk_main();
}
