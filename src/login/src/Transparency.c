// * Could change gdkrgba colors to struct
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
//     Include the header file:
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
//     enable_transparency   - Enable widget transparency
// 
//     set_color_and_opacity - Set a widget's color and opacity 
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Enable Widget Transparency
//     * Set Widget Color and Opacity
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 06 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Transparency.h"
#include "../hdr/Config.h"

#include <gtk/gtk.h>


// Declares
void enable_transparency(GtkWidget *widget);
void set_color_and_opacity(GtkWidget *window, GtkWidget *widget, 
                           const GdkRGBA bg_widget, const GdkRGBA fg_widget);
void set_stuff(struct glmstruct *gui);
void set_ass(GtkWidget *win, GtkWidget *widg, const GdkRGBA color[4]);

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



// ////////////////////////////////////////
// ///// SET WIDGET COLOR AND OPACITY /////
// ////////////////////////////////////////

// Set widget color and opacity
void set_color_and_opacity(GtkWidget *window, GtkWidget *widget, 
                           const GdkRGBA bg_widget, const GdkRGBA fg_widget) {
    
    // Define the color scheme
    const GdkRGBA bg_window = {0, 0, 0, 0};
    const GdkRGBA fg_window = {0, 0, 0, 0};
    
    // Set the color scheme
    gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &bg_window);
    gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &bg_widget);
    gtk_widget_override_color(window, GTK_STATE_FLAG_NORMAL, &fg_window);
    gtk_widget_override_color(widget, GTK_STATE_FLAG_NORMAL, &fg_widget);
}

void set_stuff(struct glmstruct *gui) {    
    
    // Set the color scheme
    gtk_widget_override_background_color(gui->win, GTK_STATE_FLAG_NORMAL, &gui->color->bgwin);
    gtk_widget_override_background_color(gui->widg, GTK_STATE_FLAG_NORMAL, &gui->color->bgwidg);
    gtk_widget_override_color(gui->win, GTK_STATE_FLAG_NORMAL, &gui->color->fgwin);
    gtk_widget_override_color(gui->widg, GTK_STATE_FLAG_NORMAL, &gui->color->fgwidg);
}

void set_ass(GtkWidget *win, GtkWidget *widg, const GdkRGBA color[4]) {
    
    // Set the color scheme
    gtk_widget_override_background_color(win, GTK_STATE_FLAG_NORMAL, &color[0]);
    gtk_widget_override_background_color(widg, GTK_STATE_FLAG_NORMAL, &color[1]);
    gtk_widget_override_color(win, GTK_STATE_FLAG_NORMAL, &color[2]);
    gtk_widget_override_color(widg, GTK_STATE_FLAG_NORMAL, &color[3]);
}
