// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Transparency.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/Transparency.h"
// 
// 
// PURPOSE:
// 
//     Enables transparency for the specified GTK widget.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     enable_transparency - Enable widget transparency
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Enable Widget Transparency
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 06 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
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
