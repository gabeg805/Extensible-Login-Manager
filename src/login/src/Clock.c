// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Clock.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "../hdr/Clock.h"
// 
// 
// PURPOSE:
// 
//     Display a clock that updates every thirty seconds.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     set_label     - Set the clock label (font, text size, and format).
// 
//     update        - Refresh the clock label (every # secs).
// 
//     display_clock - Display the clock.
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Set Clock Time
//     * Update Clock
//     * Display Clock
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 02 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
// 
//     gabeg Aug 20 2014 <> Moved the code inside Interface.c that displays the 
//                          clock into the main Clock.c module 
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries, and updated the code with 
//                          structs to remove redundant functions
// 
//     gabeg Oct 23 2014 <> Modified allocated structures so that less memory is 
//                          used. Removed the "Clock Setup" function and used the 
//                          universal setup function instead.
// 
//     gabeg Oct 31 2014 <> Changed GTK window type from TOPLEVEL to POPUP. 
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
#include "../hdr/Clock.h"

// Private functions
static void set_label(struct glmapp *app);
static gboolean update(gpointer data);
static void display_item(char *file);



// //////////////////////////
// ///// SET CLOCK TIME /////
// //////////////////////////

// Set the clock label font and text size
static void set_label(struct glmapp *app) {
    
    // Obtain current time as seconds elapsed since the Epoch.
    time_t current = time(NULL);
    char time[50];
    struct tm *tmp = localtime(&current);
    
    // Convert time
    strftime(time, sizeof(time), app->txt.fmt, tmp);
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(app->txt.font);
    PangoAttribute *attrSize = pango_attr_size_new( (long)1024 * app->txt.size );
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrFont);
    
    // Set the label text and font
    gtk_label_set_attributes(GTK_LABEL(app->widg), attrList);
    gtk_label_set_text(GTK_LABEL(app->widg), time);
}



// ////////////////////////
// ///// UPDATE CLOCK /////
// ////////////////////////

// Refresh the current clock label
static gboolean update(gpointer data) {
    struct glmapp *app = (struct glmapp *) data;
    set_label(app);
    
    return TRUE;
}



// /////////////////////////
// ///// DISPLAY CLOCK /////
// /////////////////////////

// Display a piece of the time
static void display_item(char *file) {
    
    // Log function start
    file_write(GLM_LOG, "a+", "%s: (%s:%d): Displaying clock item...", 
               __FILE__, __FUNCTION__, __LINE__);
    
    // Allocate items for the application 
    struct glmapp *app = malloc( sizeof(struct glmapp) );
    app->txt.font = malloc(READ_CHAR_LEN);
    app->txt.fmt  = malloc(READ_CHAR_LEN);
    
    // Define the application widget
    app->win  = gtk_window_new(GTK_WINDOW_POPUP);
    app->widg = gtk_label_new("");
    
    // Create the clock
    setup_widget(file, app, NULL, NULL);
    set_label(app);
    g_timeout_add_seconds(UPDATE_SEC, update, app);
    
    // Log function end
    file_write(GLM_LOG, "a+", "Done.\n");
}



// Display the date and time clock
void display_clock() {
    display_item(CLOCK_DATE_PREF);
    display_item(CLOCK_TIME_PREF);
}
