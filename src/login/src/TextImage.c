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
//     gabeg Oct 23 2014 <> Moved over #define statements from "Config.c" to here. 
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
//                          the preferences file, "set_pref_pos" and "set_pref_txt".
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
#include "../hdr/TextImage.h"

// Private functions
static void draw_text(cairo_t *cr);
static gboolean draw_window(GtkWidget *window);

// Application
static struct glmapp APP;


// ///////////////////////////
// ///// DRAW TEXT IMAGE /////
// ///////////////////////////

// Draw the text
static void draw_text(cairo_t *cr) {         
    cairo_select_font_face(cr, APP.txt.font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, APP.txt.size);
    cairo_set_source_rgb(cr, APP.txt.red, APP.txt.green, APP.txt.blue);
    cairo_move_to(cr, 0, APP.txt.size);
    cairo_show_text(cr, APP.txt.text);
}



// Draw the root window
static gboolean draw_window(GtkWidget *widg) {
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widg));
    
    // Draw the window background
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
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
    
    // Log function start
    file_write(GLM_LOG, "a+", "%s: (%s:%d): Displaying text image...", 
               __FILE__, __FUNCTION__, __LINE__);
    
    // Allocate application attributes
    APP.txt.text = malloc(READ_CHAR_LEN);
    APP.txt.font = malloc(READ_CHAR_LEN);
    
    // Define the application widget
    APP.win  = gtk_window_new(GTK_WINDOW_POPUP);
    APP.widg = gtk_drawing_area_new(); 
    
    // Create the text image
    setup_widget(TEXT_PREF, &APP, "draw", (void *)draw_window);
    
    // Log function completion
    file_write(GLM_LOG, "a+", "Done\n");
}
