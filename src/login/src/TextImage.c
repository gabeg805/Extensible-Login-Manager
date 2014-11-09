// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     TextImage.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "../hdr/TextImage.h"
// 
// 
// PURPOSE:
// 
//     Create and display the image of the word 'Password:'.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     draw_text          - Draw the password text
//     draw_text_window   - Draw the root window with all its objects
// 
//     display_text_image - Display the text image
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Draw Text Image
//     * Dislay Text Image
/// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 02 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated the header.
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries.
// 
//     gabeg Oct 23 2014 <> Moved over #define statements from "Config.c" to here. 
//                          Made "draw_..." functions static and changed the 
//                          textimage setup function for the main universal setup 
//                          funtion.
// 
//     gabeg Nov 01 2014 <> Changed the GTK window type from TOPLEVEL to POPUP. 
// 
//     gabeg Nov 08 2014 <> Moved the position of the text image 
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"
#include "../hdr/TextImage.h"
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>

#define XPOS    570
#define YPOS    325
#define FSIZE   12
#define STR     "Password:"
#define FONT    "DejaVu Sans"


// Declares
static void draw_text(cairo_t *);
static gboolean draw_text_window(GtkWidget *window);
void display_text_image();



// ///////////////////////////
// ///// DRAW TEXT IMAGE /////
// ///////////////////////////

// Draw the text
static void draw_text(cairo_t *cr) {         
    cairo_select_font_face(cr, FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, FSIZE);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, 0, FSIZE);
    cairo_show_text(cr, STR);
}



// Draw the root window
static gboolean draw_text_window(GtkWidget *window) {
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(window));
    
    // Check for window transparency
    cairo_set_source_rgba(cr, 0, 0, 0, 0); 
    
    // Draw the window background 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
        
    // Draw the login frame
    draw_text(cr);
    cairo_destroy(cr);
    
    return FALSE;
}



// //////////////////////////////
// ///// DISPLAY TEXT IMAGE /////
// //////////////////////////////

// Dislay the text image
void display_text_image() {
    
    // Initialize text image elements
    GtkWidget *win = gtk_window_new(GTK_WINDOW_POPUP);
    GtkWidget *widg = gtk_drawing_area_new();
    
    // Define structs to hold widget informationx
    int pos[4] = {XPOS, YPOS, FSIZE*strlen(STR), FSIZE+1};
    
    // Setup text image
    setup_widget(win, widg, pos, NULL);
    g_signal_connect(G_OBJECT(widg), "draw", G_CALLBACK(draw_text_window), NULL);
    
    // Display the password text image
    gtk_widget_show(widg);
    gtk_widget_show(win);
}
