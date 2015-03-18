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
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file. Included the new functions that read in from
//                          the preferences file, "set_pref_pos", "set_pref_txt", and
//                          "set_pref_decor".
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Password.h"

// Private functions
static void setup_entry(GtkWidget *entry);
static void get_entry_text(GtkWidget *entry);

static struct glmpos POS;
static struct glmtxt TXT;
static struct glmdecor DECOR;



// ///////////////////////////
// ///// SETUP ENTRY BOX /////
// ///////////////////////////

// Initialize the entry box
static void setup_entry(GtkWidget *entry) {
    
    // Set entry box attributes
    gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(entry), TXT.invis);
    
    GtkEntryBuffer *buf = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(buf, TXT.maxchars);
    gtk_entry_set_buffer(GTK_ENTRY(entry), buf);
    
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(TXT.font);
    PangoAttribute *attrSize = pango_attr_size_new(TXT.size);
    PangoAttribute *attrColor = pango_attr_foreground_new(TXT.red, TXT.green, TXT.blue);
    
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
    gtk_entry_buffer_set_max_length(buf, TXT.maxchars);
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
    
    // Allocate memory for strings
    TXT.font = malloc(READ_CHAR_LEN);
    
    // Define values in preference file    
    set_pref_pos(PASS_PREF, &POS);
    set_pref_txt(PASS_PREF, &TXT);
    set_pref_decor(PASS_PREF, &DECOR);
    
    // Initialize password entry box elements
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *widg = gtk_entry_new();
    
    // Setup password entry box
    set_widget_color(win, widg, DECOR);
    setup_widget(win, widg, POS);
    setup_entry(widg);
    g_signal_connect(G_OBJECT(widg), "activate", G_CALLBACK(get_entry_text), NULL);
    
    // Display password entry box
    gtk_widget_show(widg);
    gtk_widget_show(win);
}
