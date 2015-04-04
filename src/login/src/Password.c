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
// OPTIONS:
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
//                          the config file, "set_config_pos", "set_config_txt", and
//                          "set_config_decor".
// 
//     gabeg Mar 19 2015 <> Utilized the universal setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
// 
//     gabeg Mar 21 2015 <> Altered the pango foreground color to auto-scale by 256
//                          so that the user does not have to. Note: The pango color
//                          color is strange.
// 
//     gabeg Mar 25 2015 <> No longer have to malloc for the application txt struct.
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
    
    double bmtime = benchmark_runtime(0);
    
    // Set entry box attributes
    gtk_entry_set_visibility(GTK_ENTRY(APP.widg), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(APP.widg), APP.txt.invis[0]);
    
    GtkEntryBuffer *buf = gtk_entry_buffer_new(NULL, -1);
    gtk_entry_buffer_set_max_length(buf, APP.txt.maxchars);
    gtk_entry_set_buffer(GTK_ENTRY(APP.widg), buf);
    
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(APP.txt.font);
    PangoAttribute *attrSize = pango_attr_size_new( (long)1024 * APP.txt.size );
    PangoAttribute *attrColor = pango_attr_foreground_new( (long)256 * APP.txt.red, 
                                                           (long)256 * APP.txt.green, 
                                                           (long)256 * APP.txt.blue);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrColor);
    
    // Set the attributes
    gtk_entry_set_attributes(GTK_ENTRY(APP.widg), attrList);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// //////////////////////////
// ///// GET ENTRY TEXT /////
// //////////////////////////

// Return user entry text
static void get_entry_text(GtkWidget *widg) {
    
    double bmtime = benchmark_runtime(0);
    
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
        
        PASSWORD = (char*) text;
    }
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
} 



// //////////////////////////////////////
// ///// DISPLAY PASSWORD ENTRY BOX /////
// //////////////////////////////////////

// Display the password entry box
void display_password_entry() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Displaying password entry box...", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    // Define the application widget
    APP.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    APP.widg = gtk_entry_new();
    
    // Create the password entry box
    setup_app(PASS_CONFIG, &APP, "activate", get_entry_text);
    setup_entry();
    
    // Log function completion
    if ( VERBOSE )
        file_log("Done\n");
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}
