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
//     TextImage.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "TextImage.h"
// 
// 
// PURPOSE:
// 
//     Create and display the image of the word 'Password:'.
// 
// 
// OPTIONS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     draw_text          - Draw the password text.
//     draw_window        - Draw the root window with all its objects.
// 
//     display_text_image - Display the text image.
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
//     gabeg Oct 23 2014 <> Moved over #define statements from "Utility.c" to here. 
//                          Made "draw_..." functions static and changed the 
//                          textimage setup function for the main universal setup 
//                          funtion.
// 
//     gabeg Nov 01 2014 <> Changed the GTK window type from TOPLEVEL to POPUP. 
// 
//     gabeg Nov 08 2014 <> Moved the position of the text image 
// 
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file. Included the new functions that read in from
//                          the config file, "set_config_pos" and "set_config_txt".
// 
//     gabeg Mar 19 2015 <> Utilized the universal setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
// 
//     gabeg Mar 25 2015 <> No longer have to malloc for the application txt struct.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "TextImage.h"
#include "elysia.h"

// Private functions
static void draw_text(cairo_t *cr);
static gboolean draw_window(GtkWidget *window);

// Application
static struct elyapp APP;



// ///////////////////////////
// ///// DRAW TEXT IMAGE /////
// ///////////////////////////

// Draw the text
static void draw_text(cairo_t *cr) {         
    
    double bmtime = benchmark_runtime(0);
    
    cairo_select_font_face(cr, APP.txt.font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, APP.txt.size);
    cairo_set_source_rgb(cr, APP.txt.red, APP.txt.green, APP.txt.blue);
    cairo_move_to(cr, 0, APP.txt.size);
    cairo_show_text(cr, APP.txt.text);
    
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
    draw_text(cr);
    cairo_destroy(cr);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
    
    return FALSE;
}



// //////////////////////////////
// ///// DISPLAY TEXT IMAGE /////
// //////////////////////////////

// Dislay the text image
void display_text_image() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Displaying text image...", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    // Define the application widget
    APP.win  = gtk_window_new(GTK_WINDOW_POPUP);
    APP.widg = gtk_drawing_area_new(); 
    
    // Create the text image
    setup_app(TEXT_CONFIG, &APP, "draw", (void *)draw_window);
    
    // Log function completion
    if ( VERBOSE )
        file_log("Done\n");
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}
