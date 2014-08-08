// 
// Created By: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// Name:
// 
//     PasswordImage.c
// 
// 
// Syntax: 
// 
//     ./PasswordImage
// 
// 
// Purpose:
// 
//     Creates and displays the image of the word 'Password:'.
// 
// 
// Keywords:
// 
//     Unknown
// 
// 
// Functions:
// 
//     init_text_root      - Initialize the root window
//     draw_text           - Draw the password text
//     draw_text_window    - Draw the root window with all its objects
//     main                - Create and display the root window with all its object
// 
// 
// File Structure:
// 
//     * Includes and Declares
//     * Initialization
//     * Create Drawing
//     * Display the Root Window
// 
// 
// Modification History:
// 	
//     gabeg Aug 02 2014 <> created
// 
// **********************************************************************************


// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/TextImage.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <cairo.h>
#include <string.h>

#define   XPOS    570
#define   YPOS    330
#define   NAME    "Password"
#define   FONT    "Inconsolata"
#define   FSIZE   10


// Declares
void init_text_root(GtkWidget *window, GtkWidget *area);
void draw_text(cairo_t *);
gboolean draw_text_window(GtkWidget *window);



// //////////////////////////
// ///// INITIALIZATION /////
// //////////////////////////

// Initialize the root window and its objects
void init_text_root(GtkWidget *window, GtkWidget *area) {
    
    // Set window attributes
    gtk_window_set_title(GTK_WINDOW(window), NAME);
    gtk_window_move(GTK_WINDOW(window), XPOS, YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), FSIZE*strlen(NAME), FSIZE+1);
    
    // Add area to the root window
    gtk_container_add(GTK_CONTAINER(window), area);
    gtk_widget_set_app_paintable(window, TRUE);
    
    // Attempt to enable window transparency
    enable_transparency(window);
    
    // GTK signals
    g_signal_connect(G_OBJECT(area), "draw", G_CALLBACK(draw_text_window), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);    
}



// //////////////////////////
// ///// CREATE DRAWING /////
// //////////////////////////

// Draw the text
void draw_text(cairo_t *cr) {         
    
    // Text to display
    char text[strlen(NAME)+2];
    snprintf(text, sizeof(text), "%s: ", NAME);
    
    // Create the display box
    cairo_select_font_face (cr, FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, FSIZE);
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_move_to (cr, 0, FSIZE);
    cairo_show_text (cr, text);
}



// Draw the root window
gboolean draw_text_window(GtkWidget *window) {
    
    // Create Cairo widget for GTK window
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(window));
    
    // Check for window transparency
    if (supports_alpha) 
        // Window transparent 
        cairo_set_source_rgba(cr, 0, 0, 0, 0); 
    else 
        // Window opaque white 
        cairo_set_source_rgb(cr, 1, 1, 1); 
    
    
    // Draw the window background 
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    
    // Draw the login frame
    draw_text(cr);
    
    return FALSE;
}



// ///////////////////////////////////
// ///// DISPLAY THE ROOT WINDOW /////
// ///////////////////////////////////

// Create and the display the root window and its objects
/* int main(int argc, char *argv[]) { */

/*     // Initialize GTK toolkit */
/*     gtk_init(&argc, &argv); */
    
/*     // Define window and drawing area */
/*     GtkWidget *text_window = gtk_window_new(GTK_WINDOW_TOPLEVEL); */
/*     GtkWidget *text = gtk_drawing_area_new(); */
    
/*     // Initialize the root window and its objects */
/*     init_text_root(text_window, text); */
    
/*     // Display GTK window */
/*     gtk_widget_show(text); */
/*     gtk_widget_show(text_window); */
/*     gtk_main(); */
    
/*     return 0; */
/* } */
