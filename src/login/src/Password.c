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
//     setup_entry            - Setup the entry box.
// 
//     get_entry_text         - Return user entry text.
// 
//     display_password_entry - Display password entry box.
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
//     gabeg Mar 19 2015 <> Utilized the universal setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Password.h"

// Private functions
static void setup_entry();
static void get_entry_text(GtkWidget *widg);

// Application (if error, could be cuz of static)
static struct glmapp APP;



// ///////////////////////////
// ///// SETUP ENTRY BOX /////
// ///////////////////////////

// Setup the entry box
static void setup_entry() {
    
    // Set entry box attributes
    gtk_entry_set_visibility(GTK_ENTRY(APP.widg), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(APP.widg), APP.txt.invis);
    
    GtkEntryBuffer *buf = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(buf, APP.txt.maxchars);
    gtk_entry_set_buffer(GTK_ENTRY(APP.widg), buf);
    
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(APP.txt.font);
    PangoAttribute *attrSize = pango_attr_size_new( (long)1024 * APP.txt.size );
    PangoAttribute *attrColor = pango_attr_foreground_new(APP.txt.red, APP.txt.green, APP.txt.blue);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrColor);
    
    // Set the attributes
    gtk_entry_set_attributes(GTK_ENTRY(APP.widg), attrList);
}



// //////////////////////////
// ///// GET ENTRY TEXT /////
// //////////////////////////

// Return user entry text
static void get_entry_text(GtkWidget *widg) {
    
    // Get the text from the entry buffer
    GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY(APP.widg));
    const gchar *text =	gtk_entry_buffer_get_text(buf);
    
    // Check the length (make sure enter wasn't accidentally hit)
    guint len = gtk_entry_buffer_get_length(buf);
    
    // Reset the buffer
    buf = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(buf, APP.txt.maxchars);
    gtk_entry_set_buffer(GTK_ENTRY(APP.widg), buf);
    
    // Quit the entry widget
    if ( len != 0 ) {
        gtk_widget_destroy(APP.widg);
        gtk_main_quit();
        
        PASSWORD = (char*)text;
    }
} 



// //////////////////////////////////////
// ///// DISPLAY PASSWORD ENTRY BOX /////
// //////////////////////////////////////

// Display the password entry box
void display_password_entry() {
    
    // Log function start
    file_write(GLM_LOG, "a+", "%s: (%s:%d): Displaying password entry box...", 
               __FILE__, __FUNCTION__, __LINE__);
    
    // Allocate application attributes
    APP.txt.font = malloc(READ_CHAR_LEN);
    
    // Define the application widget
    APP.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    APP.widg = gtk_entry_new();
    
    // Create the password entry box
    setup_widget(PASS_PREF, &APP, "activate", get_entry_text);
    setup_entry();
    
    // Log function completion
    file_write(GLM_LOG, "a+", "Done\n");
}
