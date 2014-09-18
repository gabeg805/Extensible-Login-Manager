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
//     init_frame_root   - Initialize the root window and all its objects
// 
//     draw_frame_window - Draw the root window
//     draw_frame        - Draw the login frame
// 
//     display_frame     - Display the login frame
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Frame
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
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Frame.h"
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>


// Declares
void init_frame_root(GtkWidget *window, GtkWidget *area);
void draw_frame(cairo_t *);
gboolean draw_frame_window(GtkWidget *widget);
void display_frame();



// ////////////////////////////
// ///// INITIALIZE FRAME /////
// ////////////////////////////

// Initialize the root window and all its objects
void init_frame_root(GtkWidget *window, GtkWidget *area) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(window), FRAME_XPOS, FRAME_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), FRAME_WIDTH+2, FRAME_HEIGHT+2);
    
    // Add area to window
    gtk_container_add(GTK_CONTAINER(window), area);
    /* gtk_widget_set_app_paintable(window, TRUE); */
    
    // Attempt to enable window transparency
    enable_transparency(window);
    
    // GTK signals
    g_signal_connect(G_OBJECT(area), "draw", G_CALLBACK(draw_frame_window), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// //////////////////////
// ///// DRAW FRAME /////
// //////////////////////

// Draw the root window 
gboolean draw_frame_window(GtkWidget *widget) {
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
    
    // Check for window transparency
    if (supports_alpha)
        cairo_set_source_rgba(cr, 0, 0, 0, 0);
    else
        cairo_set_source_rgb(cr, 1, 1, 1);
    
    
    // Draw the window background
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    
    // Draw the login frame
    draw_frame(cr);
    cairo_destroy(cr);
    
    return FALSE;
}



// Draw the login frame
void draw_frame(cairo_t *cr) { 
    
    // Custom shape that could be wrapped in a function 
    double x             = 1, 
           y             = 1,
           aspect        = 1.0, 
           corner_radius = FRAME_HEIGHT / 10.0; 
    
    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;
    
    // Create the rouded rectangle
    cairo_set_line_width (cr, 0);
    cairo_new_sub_path(cr);
    cairo_arc(cr,   x+FRAME_WIDTH-radius,   y+radius,                radius,   -90*degrees,     0*degrees);
    cairo_arc(cr,   x+FRAME_WIDTH-radius,   y+FRAME_HEIGHT-radius,   radius,     0*degrees,    90*degrees);
    cairo_arc(cr,   x+radius,               y+FRAME_HEIGHT-radius,   radius,    90*degrees,   180*degrees);
    cairo_arc(cr,   x+radius,               y+radius,                radius,   180*degrees,   270*degrees);
    cairo_close_path (cr);
    
    // Check for window transparency
    if (supports_alpha) 
        cairo_set_source_rgba(cr, 1, 1, 1, 0.5);
    else 
        cairo_set_source_rgb(cr, 1, 1, 1); 
    
    // Fill login frame
    cairo_fill_preserve(cr);
    cairo_stroke (cr);
}



// /////////////////////////
// ///// DISPLAY FRAME /////
// /////////////////////////

// Display the login frame
void display_frame() {
    
    // Initialize frame elements
    GtkWidget *frame_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *frame = gtk_drawing_area_new();
    
    // Setup frame
    init_frame_root(frame_window, frame);
    
    // Display the login frame
    gtk_widget_show(frame);
    gtk_widget_show(frame_window);
}
