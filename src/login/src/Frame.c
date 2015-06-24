// 
// CONTRIBUTORS: 
// 
//     * Gabriel Gonzalez (gabeg@bu.edu) 
// 
// 
// LICENSE: 
// 
//     The MIT License (MIT)
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
//         #include "Frame.h"
// 
// 
// PURPOSE:
// 
//     Create a frame for the login items.
// 
// 
// OPTIONS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     draw_frame    - Draw the login frame.
//     draw_window   - Draw the root window.
// 
//     display_frame - Display the login frame.
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
//     gabeg Oct 31 2014 <> Changed GTK window type from TOPLEVEL to POPUP. 
// 
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file. Included the new functions that read in from
//                          the config file, "set_config_pos".
// 
//     gabeg Mar 19 2015 <> Utilized the universal setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "Frame.h"
#include "elysia.h"

// Private functions
static void draw_frame(cairo_t *cr);
static gboolean draw_window(GtkWidget *widget);

// Declares
static struct elyapp APP;



// //////////////////////
// ///// DRAW FRAME /////
// //////////////////////

// Draw the login frame
static void draw_frame(cairo_t *cr) { 
    
    double bmtime = benchmark_runtime(0);
    
    // Custom shape that could be wrapped in a function 
    double width = APP.pos.width,
        height   = APP.pos.height,
        rad      = height / 5.0,
        deg      = M_PI / 180.0;
    
    // Create the rouded rectangle
    cairo_set_line_width (cr, 0);
    cairo_new_sub_path(cr);
    cairo_arc(cr, width-rad, rad,        rad, -90*deg,   0*deg);
    cairo_arc(cr, width-rad, height-rad, rad,   0*deg,  90*deg);
    cairo_arc(cr, rad,       height-rad, rad,  90*deg, 180*deg);
    cairo_arc(cr, rad,       rad,        rad, 180*deg, 270*deg);
    cairo_close_path (cr);
    
    // Check for window transparency
    cairo_set_source_rgba(cr, 1, 1, 1, 0.5);
    
    // Fill login frame
    cairo_fill_preserve(cr);
    cairo_stroke (cr);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}




// Draw the root window 
static gboolean draw_window(GtkWidget *widg) {
    
    double bmtime = benchmark_runtime(0);
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widg));
    
    // Draw the window background
    cairo_set_source_rgba(cr, 0, 0, 0, 0);    
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    
    // Draw the login frame
    draw_frame(cr);
    cairo_destroy(cr);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
    
    return FALSE;
}



// /////////////////////////
// ///// DISPLAY FRAME /////
// /////////////////////////

// Display the login frame
void display_frame() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Displaying login frame...",
                 __FILE__, __FUNCTION__, __LINE__);
    
    // Define the application widget
    APP.win  = gtk_window_new(GTK_WINDOW_POPUP);
    APP.widg = gtk_drawing_area_new(); 
    
    // Create the login frame
    setup_app(FRAME_CONFIG, &APP, "draw", (void *)draw_window);
    
    // Log function completion
    if ( VERBOSE )
        file_log("Done\n");
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}

