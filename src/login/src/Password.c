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
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/Password.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o Password Password.c Transparency.c `pkg-config gtk+-3.0 --cflags --libs`
//         $ ./Password
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
//     init_entry_root - Initialize the root window
//     init_entry      - Initialize the entry box
// 
//     get_entry_text  - Return user entry text
// 
//     password_entry  - Display the password entry box and return the input password
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Entry Box
//     * Get Entry Text
//     * Display Entry Box
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 03 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Password.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define   XPOS        570
#define   YPOS        350
#define   WIDTH       230
#define   HEIGHT      30
#define   FONT        "Inconsolata"
#define   FSIZE       10*1024
#define   INVISCHAR   '*'
#define   MAXCHARS    30


// Declares
void init_entry_root(GtkWidget *window, GtkWidget *entry);
void init_entry(GtkWidget *entry);
void get_entry_text(GtkWidget *entry);
char * password_entry(int argc, char *argv[]);

char output[1024];



// ////////////////////////////////
// ///// INITIALIZE ENTRY BOX /////
// ////////////////////////////////

// Initialize the root window
void init_entry_root(GtkWidget *window, GtkWidget *entry) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(window), XPOS, YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT*0);
    
    // Set color scheme for root window
    const GdkRGBA bg_widget = {1, 1, 1, 0.5};
    const GdkRGBA fg_widget = {0, 0, 0, 1};
    set_color_and_opacity(window, entry, bg_widget, fg_widget);
    
    // Add entry to window
    gtk_container_add(GTK_CONTAINER(window), entry);
    
    // Enable transparency
    enable_transparency(window);
    
    // GTK signals
    g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(get_entry_text), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);    
}



// Initialize the entry box
void init_entry(GtkWidget *entry) {

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
void get_entry_text(GtkWidget *entry) {
    
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
        gtk_main_quit();
        snprintf(output, len+1, text);
    }
}        



// /////////////////////////////
// ///// DISPLAY ENTRY BOX /////
// /////////////////////////////

// Display the password entry box
char * password_entry(int argc, char *argv[]) {
    
    // Initialize GTK toolkit
    gtk_init(&argc, &argv);

    // Define window and entry box
    GtkWidget *entry_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *entry = gtk_entry_new();
        
    // Initialize root window and entry box
    init_entry_root(entry_window, entry);
    init_entry(entry);
    
    // Display entry box
    gtk_widget_show(entry);
    gtk_widget_show(entry_window);
    
    gtk_main();
    
    // Allocate memory for password output 
    char *pass = malloc(sz);
    snprintf(pass, strlen(output), output);
    
    return pass;
}
