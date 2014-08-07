// 
// Created By: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// Name:
// 
//     Transparency.c
// 
// 
// Syntax: 
// 
//     #include "Transparency.h"
// 
// 
// Purpose:
// 
//     Enables transparency for the specified GTK widget.
// 
// 
// Keywords:
// 
//     Unknown
// 
// 
// Functions:
// 
//     main - the main function
// 
// 
// File Structure:
// 
//     * Includes and Declares
//     * Objective
// 
// 
// Modification History:
// 	
//     gabeg MON DAY YEAR <> created
// 
// **********************************************************************************


// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>

// Declares
void enable_transparency(GtkWidget *widget);

gboolean supports_alpha = FALSE;



// //////////////////////////////////////
// ///// ENABLE WIDGET TRANSPARENCY ///// 
// //////////////////////////////////////

// Enable widget transparency
void enable_transparency(GtkWidget *widget) {
    
    // To check if the display supports alpha channels, get the visual 
    GdkScreen *screen = gtk_widget_get_screen(widget);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    
    // Check if visual executed properly
    if (!visual) {
        visual = gdk_screen_get_system_visual(screen);
        supports_alpha = FALSE;
    } else
        supports_alpha = TRUE;
    
    // Set widget visual
    gtk_widget_set_visual(widget, visual);
}
