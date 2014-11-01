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
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"
#include "../hdr/Clock.h"
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#define DATE_XPOS    50
#define DATE_YPOS    650
#define DATE_WIDTH   0
#define DATE_HEIGHT  0
#define TIME_XPOS    110
#define TIME_YPOS    575
#define TIME_WIDTH   0
#define TIME_HEIGHT  0
#define BG_WINDOW    (const GdkRGBA) {0, 0, 0, 0}
#define BG_DATE      (const GdkRGBA) {0, 0, 0, 0}
#define BG_TIME      (const GdkRGBA) {0, 0, 0, 0}
#define FG_WINDOW    (const GdkRGBA) {0, 0, 0, 0}
#define FG_DATE      (const GdkRGBA) {1, 1, 1, 1}
#define FG_TIME      (const GdkRGBA) {1, 1, 1, 1}
#define DATE_FSIZE   25*1024
#define TIME_FSIZE   45*1024
#define DATE_FONT    "Inconsolata"
#define TIME_FONT    "Inconsolata"
#define DATE_FMT     "%A, %B %-d"
#define TIME_FMT     "%-I:%M %p"
#define UPDATE_SEC   30


// Definitions
struct glmtext {
    GtkWidget *widg;
    char *font;
    char *fmt;
    int size;
};


// Declares
static struct glmtext * setup_text_struct(GtkWidget *widg, char *font, char *fmt, int size);
static void set_label(struct glmtext *gui);
static gboolean update(gpointer data);
void display_clock();



// //////////////////////////////////
// ///// SETUP TEXT INFO STRUCT /////
// //////////////////////////////////

// Setup the GUI text struct
static struct glmtext * setup_text_struct(GtkWidget *widg, char *font, char *fmt, int size) {
    
    struct glmtext *text = malloc(sizeof(struct glmtext));
    assert(text);
    
    text->widg = widg;
    text->font = font;
    text->fmt = fmt;
    text->size = size;
    
    return text;
}



// //////////////////////////
// ///// SET CLOCK TIME /////
// //////////////////////////

// Set the clock label font and text size
static void set_label(struct glmtext *gui) {
    
    // Obtain current time as seconds elapsed since the Epoch.
    time_t current = time(NULL);
    char time[50];
    struct tm *tmp = localtime(&current);
    
    // Convert time
    strftime(time, sizeof(time), gui->fmt, tmp);
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(gui->font);
    PangoAttribute *attrSize = pango_attr_size_new(gui->size);
    
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
    struct glmtext *gui = (struct glmtext *) data;
    set_label(gui);
    
    return TRUE;
}



// /////////////////////////
// ///// DISPLAY CLOCK /////
// /////////////////////////

// Display the date and time clock
void display_clock() {
    
    // Initialize date and time widgets
    GtkWidget *date_win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *time_win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *date_widg = gtk_label_new("");
    GtkWidget *time_widg = gtk_label_new("");
    
    
    // Define structs to hold widget information
    int date_pos[4] = {DATE_XPOS, DATE_YPOS, DATE_WIDTH, DATE_HEIGHT};
    int time_pos[4] = {TIME_XPOS, TIME_YPOS, TIME_WIDTH, TIME_HEIGHT};
    
    const GdkRGBA date_color[4] = {BG_WINDOW, BG_DATE, FG_WINDOW, FG_DATE};
    const GdkRGBA time_color[4] = {BG_WINDOW, BG_TIME, FG_WINDOW, FG_TIME};
    
    struct glmtext *date_text = setup_text_struct(date_widg, DATE_FONT, DATE_FMT, DATE_FSIZE);
    struct glmtext *time_text = setup_text_struct(time_widg, TIME_FONT, TIME_FMT, TIME_FSIZE);
    
    
    // Setup date and time clocks 
    setup_widget(date_win, date_widg, date_pos, date_color);
    setup_widget(time_win, time_widg, time_pos, time_color);
    
    set_label(date_text);
    set_label(time_text);
    
    g_timeout_add_seconds(UPDATE_SEC, update, time_text);
    g_timeout_add_seconds(UPDATE_SEC, update, date_text);
    
    
    // Display the clock
    gtk_widget_show(date_widg);
    gtk_widget_show(time_widg);
    gtk_widget_show(date_win);
    gtk_widget_show(time_win);
}
