// 
// Created By: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// Name:
// 
//     Frame.c
// 
// 
// Syntax: 
// 
//     ./Frame
// 
// 
// Purpose:
// 
//     Creates a frame for the login items.
// 
// 
// Keywords:
// 
//     Unknown
// 
// 
// Functions:
// 
//     init_frame_root     - Initialize the root window and all its objects
//     draw_frame_window   - Draw the root window
//     draw_frame          - Draw the login frame
//     main                - Display the login frame
// 
// 
// File Structure:
// 
//     * Includes and Declares
//     * Initialization
//     * Drawing
//     * Display the Login Frame
// 
// 
// Modification History:
// 	
//     gabeg Aug 03 2014 <> created
// 
// **********************************************************************************


// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Frame.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>

#define   XPOS      550
#define   YPOS      300
#define   FWIDTH    265
#define   FHEIGHT   100


// Declares
void init_frame_root(GtkWidget *window, GtkWidget *area);
void draw_frame(cairo_t *);
gboolean draw_frame_window(GtkWidget *widget);



// //////////////////////////
// ///// INITIALIZATION /////
// //////////////////////////

// Initialize the root window and all its objects
void init_frame_root(GtkWidget *window, GtkWidget *area) {
    
    // Set window attributes
    gtk_window_set_title(GTK_WINDOW(window), "Login Frame");
    gtk_window_move(GTK_WINDOW(window), XPOS, YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), FWIDTH+2, FHEIGHT+2);
    
    // Add area to window
    gtk_container_add(GTK_CONTAINER(window), area);
    gtk_widget_set_app_paintable(window, TRUE);

    // Attempt to enable window transparency
    enable_transparency(window);
    
    // GTK signals
    g_signal_connect(G_OBJECT(area), "draw", G_CALLBACK(draw_frame_window), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// ///////////////////
// ///// DRAWING /////
// ///////////////////

// Draw the root window 
gboolean draw_frame_window(GtkWidget *widget) {
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
    
    // Check for window transparency
    if (supports_alpha) 
        // Window transparent 
        cairo_set_source_rgba(cr, 0.5, 1.0, 0.50, 0); 
    else 
        // Window opaque white 
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); 
    
    
    // Draw the window background 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    
    // Draw the login frame
    draw_frame(cr);
    
    return FALSE;
}



// Draw the login frame
void draw_frame(cairo_t *cr) { 
    
    // Custom shape that could be wrapped in a function 
    double x             = 1, 
           y             = 1,
           aspect        = 1.0, 
           corner_radius = FHEIGHT / 10.0; 
    
    double radius = corner_radius / aspect;
    double degrees = M_PI / 180.0;
    
    // Create the rouded rectangle
    cairo_set_line_width (cr, 0);
    cairo_new_sub_path(cr);
    cairo_arc(cr,   x+FWIDTH-radius,   y+radius,           radius,   -90*degrees,     0*degrees);
    cairo_arc(cr,   x+FWIDTH-radius,   y+FHEIGHT-radius,   radius,     0*degrees,    90*degrees);
    cairo_arc(cr,   x+radius,          y+FHEIGHT-radius,   radius,    90*degrees,   180*degrees);
    cairo_arc(cr,   x+radius,          y+radius,           radius,   180*degrees,   270*degrees);
    cairo_close_path (cr);
    
    // Check for window transparency
    if (supports_alpha) 
        // Window transparent
        cairo_set_source_rgba(cr, 0.5, 0.5, 1, 0.5);
    else 
        // Window opaque white
        cairo_set_source_rgb(cr, 1, 1, 1); 
    
    // Fill login frame
    cairo_fill_preserve(cr);
    cairo_stroke (cr);
}



// ///////////////////////////////////
// ///// DISPLAY THE LOGIN FRAME /////
// ///////////////////////////////////

// Display the login frame
/* int main(int argc, char *argv[]) { */
    
/*     // Initialize GTK toolkit */
/*     gtk_init(&argc, &argv); */
    
/*     // Create window and drawing area */
/*     GtkWidget *frame_window = gtk_window_new(GTK_WINDOW_TOPLEVEL); */
/*     GtkWidget *frame = gtk_drawing_area_new(); */
    
/*     // Initialize the root window with all its objects */
/*     init_frame_root(frame_window, frame); */
    
/*     // Display GTK window */
/*     gtk_widget_show(frame); */
/*     gtk_widget_show(frame_window); */
/*     gtk_main(); */
    
/*     return 0; */
/* } */
