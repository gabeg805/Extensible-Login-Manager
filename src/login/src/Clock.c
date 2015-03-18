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
//     setup_text_struct - Set the GUI text struct 
// 
//     set_label         - Set the clock label (font, text size, and format)
// 
//     update            - Refresh the clock label (every # secs)
// 
//     display_clock     - Display the clock
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Setup Text Info Struct
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
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Clock.h"

// Private functions
static void set_label(struct glmtxt_w *gui);
static gboolean update(gpointer data);

static struct glmpos DATE_POS;
static struct glmtxt DATE_TXT;
static struct glmdecor DATE_DECOR;

static struct glmpos TIME_POS;
static struct glmtxt TIME_TXT;
static struct glmdecor TIME_DECOR;



// //////////////////////////
// ///// SET CLOCK TIME /////
// //////////////////////////

// Set the clock label font and text size
static void set_label(struct glmtxt_w *gui) {
    
    // Obtain current time as seconds elapsed since the Epoch.
    time_t current = time(NULL);
    char time[50];
    struct tm *tmp = localtime(&current);
    
    // Convert time
    strftime(time, sizeof(time), gui->txt.fmt, tmp);
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(gui->txt.font);
    PangoAttribute *attrSize = pango_attr_size_new(gui->txt.size);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrFont);
    
    // Set the label text and font
    gtk_label_set_attributes(GTK_LABEL(gui->widg), attrList);
    gtk_label_set_text(GTK_LABEL(gui->widg), time);
}



// ////////////////////////
// ///// UPDATE CLOCK /////
// ////////////////////////

// Refresh the current clock label
static gboolean update(gpointer data) {
    struct glmtxt_w *gui = (struct glmtxt_w *) data;
    set_label(gui);
    
    return TRUE;
}



// /////////////////////////
// ///// DISPLAY CLOCK /////
// /////////////////////////

// Display the date and time clock
void display_clock() {
    
    // Allocate memory for strings
    DATE_TXT.font = malloc(READ_CHAR_LEN);
    DATE_TXT.fmt  = malloc(READ_CHAR_LEN);
    
    TIME_TXT.font = malloc(READ_CHAR_LEN);
    TIME_TXT.fmt  = malloc(READ_CHAR_LEN);
    
    // Define values in preference file    
    set_pref_pos(CLOCK_DATE_PREF, &DATE_POS);
    set_pref_txt(CLOCK_DATE_PREF, &DATE_TXT);
    set_pref_decor(CLOCK_DATE_PREF, &DATE_DECOR);
    
    set_pref_pos(CLOCK_TIME_PREF, &TIME_POS);
    set_pref_txt(CLOCK_TIME_PREF, &TIME_TXT);
    set_pref_decor(CLOCK_TIME_PREF, &TIME_DECOR);
    
    // !@#)(!@*# CAN SHORTEN THIS INTO ANOTHER FUNC Q)(@$#)(@*
    
    // Initialize date and time widgets
    GtkWidget *date_win  = gtk_window_new(GTK_WINDOW_POPUP);
    GtkWidget *date_widg = gtk_label_new("");
    
    GtkWidget *time_win  = gtk_window_new(GTK_WINDOW_POPUP);
    GtkWidget *time_widg = gtk_label_new("");
    
    
    // Pack together widget with text formatting struct
    struct glmtxt_w *date_pack = malloc( sizeof(struct glmtxt_w) );
    struct glmtxt_w *time_pack = malloc( sizeof(struct glmtxt_w) );
    
    date_pack->widg = date_widg;
    date_pack->txt  = DATE_TXT;
    
    time_pack->widg = time_widg;
    time_pack->txt  = TIME_TXT;
    
    
    // Setup date and time clocks 
    set_widget_color(date_win, date_widg, DATE_DECOR);
    set_widget_color(time_win, time_widg, TIME_DECOR);
    
    setup_widget(date_win, date_widg, DATE_POS);
    setup_widget(time_win, time_widg, TIME_POS);
    
    set_label(date_pack);
    set_label(time_pack);
    
    g_timeout_add_seconds(UPDATE_SEC, update, date_pack);
    g_timeout_add_seconds(UPDATE_SEC, update, time_pack);
    
    
    // Display the clock
    gtk_widget_show(date_widg);
    gtk_widget_show(time_widg);
    gtk_widget_show(date_win);
    gtk_widget_show(time_win);
}
