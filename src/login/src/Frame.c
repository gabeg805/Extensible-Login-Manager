// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Frame.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "../hdr/Frame.h"
// 
// 
// PURPOSE:
// 
//     Create a frame for the login items.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     draw_frame      - Draw the login frame
//     draw_window     - Draw the root window
// 
//     display_frame   - Display the login frame
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Draw Frame
//     * Display Frame
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 03 2014 <> created
// 
//     gabeg Aug 03 2014 <> Updated the header
// 
//     gabeg Aug 20 2014 <> Moved the code inside Interface.c that displays the 
//                          login frame into the main Frame.c module 
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries 
// 
//     gabeg Sep 18 2014 <> Updated code to hold structs, to increase readability 
// 
//     gabeg Oct 23 2014 <> Removed check for alpha color support. Removed the "Frame 
//                          Setup" function and used the universal setup function 
//                          instead.
// 
//     gabeg Oct 31 2014 <> Changed the GTK window from TOPLEVEL to POPUP. 
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"
#include "../hdr/Frame.h"
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>

// Add colors
#define XPOS     550
#define YPOS     300
#define WIDTH    267
#define HEIGHT   102

// Declares
static void draw_frame(cairo_t *);
static gboolean draw_window(GtkWidget *widget);
void display_frame();



// //////////////////////
// ///// DRAW FRAME /////
// //////////////////////

// Draw the login frame
static void draw_frame(cairo_t *cr) { 
    
    // Custom shape that could be wrapped in a function 
    double radius = HEIGHT / 5.0;
    double degrees = M_PI / 180.0;
    
    // Create the rouded rectangle
    cairo_set_line_width (cr, 0);
    cairo_new_sub_path(cr);
    cairo_arc(cr,   WIDTH-radius,   radius,          radius,   -90*degrees,     0*degrees);
    cairo_arc(cr,   WIDTH-radius,   HEIGHT-radius,   radius,     0*degrees,    90*degrees);
    cairo_arc(cr,   radius,         HEIGHT-radius,   radius,    90*degrees,   180*degrees);
    cairo_arc(cr,   radius,         radius,          radius,   180*degrees,   270*degrees);
    cairo_close_path (cr);
    
    // Check for window transparency
    cairo_set_source_rgba(cr, 1, 1, 1, 0.5);
    
    // Fill login frame
    cairo_fill_preserve(cr);
    cairo_stroke (cr);
}




// Draw the root window 
static gboolean draw_window(GtkWidget *widget) {
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
    
    // Draw the window background
    cairo_set_source_rgba(cr, 0, 0, 0, 0);    
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    
    // Draw the login frame
    draw_frame(cr);
    cairo_destroy(cr);
    
    return FALSE;
}



// /////////////////////////
// ///// DISPLAY FRAME /////
// /////////////////////////

// Display the login frame
void display_frame() {
    
    // Initialize date gui widget
    GtkWidget *win = gtk_window_new(GTK_WINDOW_POPUP);
    GtkWidget *widg = gtk_drawing_area_new();
    
    // Setup structs to hold widget information
    int pos[4] = {XPOS, YPOS, WIDTH, HEIGHT};
    
    // Setup frame
    setup_widget(win, widg, pos, NULL);
    g_signal_connect(G_OBJECT(widg), "draw", G_CALLBACK(draw_window), NULL);
    
    // Display the login frame
    gtk_widget_show(widg);
    gtk_widget_show(win);
}
