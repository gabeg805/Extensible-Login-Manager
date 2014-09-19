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
//     setup           - Setup the login frame widget
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
//     gabeg Sep 18 2014 <> Updated code to hold structs, to increase readability 
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Frame.h"
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"
#include "../hdr/Utility.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>

// Add colors
#define FRAME_XPOS     550
#define FRAME_YPOS     300
#define FRAME_WIDTH    267
#define FRAME_HEIGHT   102


// Declares
static void setup(struct glmgui *frame);
static void draw_frame(cairo_t *);
static gboolean draw_window(GtkWidget *widget);
void display_frame();



// ////////////////////////////
// ///// INITIALIZE FRAME /////
// ////////////////////////////

// Initialize the root window and all its objects
static void setup(struct glmgui *gui) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(gui->win), gui->pos->x, gui->pos->y);
    gtk_window_set_default_size(GTK_WINDOW(gui->win), gui->pos->width, gui->pos->height);
    
    // Add area to window
    gtk_container_add(GTK_CONTAINER(gui->win), gui->widg);
    
    // Attempt to enable window transparency
    enable_transparency(gui->win);
    
    // GTK signals
    g_signal_connect(G_OBJECT(gui->widg), "draw", G_CALLBACK(draw_window), NULL);
    g_signal_connect(gui->win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// //////////////////////
// ///// DRAW FRAME /////
// //////////////////////

// Draw the login frame
static void draw_frame(cairo_t *cr) { 
    
    // Custom shape that could be wrapped in a function 
    double radius = FRAME_HEIGHT / 5.0;
    double degrees = M_PI / 180.0;
    
    // Create the rouded rectangle
    cairo_set_line_width (cr, 0);
    cairo_new_sub_path(cr);
    cairo_arc(cr,   FRAME_WIDTH-radius,   radius,                radius,   -90*degrees,     0*degrees);
    cairo_arc(cr,   FRAME_WIDTH-radius,   FRAME_HEIGHT-radius,   radius,     0*degrees,    90*degrees);
    cairo_arc(cr,   radius,               FRAME_HEIGHT-radius,   radius,    90*degrees,   180*degrees);
    cairo_arc(cr,   radius,               radius,                radius,   180*degrees,   270*degrees);
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




// Draw the root window 
static gboolean draw_window(GtkWidget *widget) {
    
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



// /////////////////////////
// ///// DISPLAY FRAME /////
// /////////////////////////

// Display the login frame
void display_frame() {
    
    // Initialize date gui widget
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *widget = gtk_drawing_area_new();
    
    // Define structs to hold GUI information
    struct glmpos *pos = setup_pos_struct(FRAME_XPOS, FRAME_YPOS, FRAME_WIDTH, FRAME_HEIGHT);
    struct glmgui *gui = setup_gui_struct(window, widget, pos, NULL, NULL);
    
    // Setup frame
    setup(gui);
    
    // Display the login frame
    gtk_widget_show(gui->widg);
    gtk_widget_show(gui->win);
}
