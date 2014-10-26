// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Password.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "../hdr/Password.h"
// 
// 
// PURPOSE:
// 
//     Display an entry box in which the user may enter their password.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     setup_entry            - Initialize the entry box
// 
//     get_entry_text         - Return user entry text
// 
//     display_password_entry - Display password entry box
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Setup Entry Box
//     * Get Entry Text
//     * Display Password Entry Box
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 03 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
// 
//     gabeg Aug 20 2014 <> Moved the code inside Interface.c that displays the 
//                          password entry box into the main Password.c module 
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries
// 
//     gabeg Oct 23 2014 <> Removed "Password Setup" function and used the universal 
//                          setup function instead.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"
#include "../hdr/Password.h"
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define XPOS        570
#define YPOS        350
#define WIDTH       230
#define HEIGHT      0
#define BG_ENTRY    (const GdkRGBA) {1, 1, 1, 0.5}
#define BG_WINDOW   (const GdkRGBA) {0, 0, 0, 0}
#define FG_WINDOW   (const GdkRGBA) {0, 0, 0, 0}
#define FG_ENTRY    (const GdkRGBA) {0, 0, 0, 1}
#define FSIZE       10*1024
#define FONT        "Inconsolata"
#define MAXCHARS    30
#define INVISCHAR   '*'


// Declares
static void setup_entry(GtkWidget *entry);
static void get_entry_text(GtkWidget *entry);
void display_password_entry();



// ///////////////////////////
// ///// SETUP ENTRY BOX /////
// ///////////////////////////

// Initialize the entry box
static void setup_entry(GtkWidget *entry) {
    
    // Set entry box attributes
    gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entry), INVISCHAR);
    
    GtkEntryBuffer *buf = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(buf, MAXCHARS);
    gtk_entry_set_buffer(GTK_ENTRY(entry), buf);
    
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(FONT);
    PangoAttribute *attrSize = pango_attr_size_new(FSIZE);
    PangoAttribute *attrColor = pango_attr_foreground_new(0, 0, 0);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrColor);
    
    // Set the attributes
    gtk_entry_set_attributes(GTK_ENTRY(entry), attrList);
}



// //////////////////////////
// ///// GET ENTRY TEXT /////
// //////////////////////////

// Return user entry text
static void get_entry_text(GtkWidget *entry) {
    
    // Get the text from the entry buffer
    GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY(entry));
    const gchar *text =	gtk_entry_buffer_get_text(buf);
    
    // Check the length (make sure enter wasn't accidentally hit)
    guint len = gtk_entry_buffer_get_length(buf);
    
    // Reset the buffer
    buf = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(buf, MAXCHARS);
    gtk_entry_set_buffer(GTK_ENTRY(entry), buf);
    
    // Quit the entry widget
    if ( len != 0 ) {
        gtk_widget_destroy(entry);
        gtk_main_quit();
        
        PASSWORD = (char*)text;
    }
} 



// //////////////////////////////////////
// ///// DISPLAY PASSWORD ENTRY BOX /////
// //////////////////////////////////////

// Display the password entry box
void display_password_entry() {
    
    // Initialize password entry box elements
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *widg = gtk_entry_new();
    
    // Define struct to hold widget information
    int pos[4] = {XPOS, YPOS, WIDTH, HEIGHT};
    const GdkRGBA color[4] = {BG_WINDOW, BG_ENTRY, FG_WINDOW, FG_ENTRY};
    
    // Setup password entry box
    setup_widget(win, widg, pos, color);
    setup_entry(widg);
    g_signal_connect(G_OBJECT(widg), "activate", G_CALLBACK(get_entry_text), NULL);
    
    // Display password entry box
    gtk_widget_show(widg);
    gtk_widget_show(win);
}
