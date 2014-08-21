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
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/Clock.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o Clock Clock.c Transparency.c `pkg-config gtk+-3.0 --cflags --libs`
//         $ ./Clock
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
//     init_clock_date_root - Initialize the clock window that displays the date
//     init_clock_time_root - Initialize the clock window that displays the time
// 
//     set_clock_label      - Set the clock label font and text size
// 
//     clock_date_update    - Refresh the date clock label
//     clock_time_update    - Refresh the time clock label
// 
//     display_clock        - Display the clock
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Date and Time Clock
//     * Set Current Time
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
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Clock.h"
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <time.h>


// Declares
void init_clock_date_root(GtkWidget *window, GtkWidget *clock);
void init_clock_time_root(GtkWidget *window, GtkWidget *clock);
void set_clock_label(GtkWidget *clock, char *fmt, const char *font, int fsize);
gboolean clock_date_update(gpointer data);
gboolean clock_time_update(gpointer data);
void display_clock();



// //////////////////////////////////////////
// ///// INITIALIZE DATE AND TIME CLOCK ///// 
// //////////////////////////////////////////

// Initialize the root window with all its objects
void init_clock_date_root(GtkWidget *window, GtkWidget *clock) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(window), CLOCK_DATE_XPOS, CLOCK_DATE_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), CLOCK_DATE_WIDTH*0, CLOCK_DATE_HEIGHT*0);
    
    // Set clock attributes
    const GdkRGBA bg_widget = {0, 0, 0, 0};
    const GdkRGBA fg_widget = {1, 1, 1, 1};
    set_color_and_opacity(window, clock, bg_widget, fg_widget);
    set_clock_label(clock, CLOCK_DATE_FMT, CLOCK_DATE_FONT, CLOCK_DATE_FSIZE);
    
    // Add the clock to the root window 
    gtk_container_add(GTK_CONTAINER(window), clock);
    g_timeout_add_seconds(CLOCK_UPDATE_SEC, clock_date_update, clock);
    
    // Enable transparency
    enable_transparency(window);
    
    // GTK signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// Initialize the root window with all its objects
void init_clock_time_root(GtkWidget *window, GtkWidget *clock) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(window), CLOCK_TIME_XPOS, CLOCK_TIME_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), CLOCK_TIME_WIDTH*0, CLOCK_TIME_HEIGHT*0);
    
    // Set clock attributes
    const GdkRGBA bg_widget = {0, 0, 0, 0};
    const GdkRGBA fg_widget = {1, 1, 1, 1};
    set_color_and_opacity(window, clock, bg_widget, fg_widget);
    set_clock_label(clock, CLOCK_TIME_FMT, CLOCK_TIME_FONT, CLOCK_TIME_FSIZE);
    
    // Add the clock to the root window 
    gtk_container_add(GTK_CONTAINER(window), clock);
    g_timeout_add_seconds(CLOCK_UPDATE_SEC, clock_time_update, clock);
    
    // Enable transparency
    enable_transparency(window);
    
    // GTK signal
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// ////////////////////////////
// ///// SET CURRENT TIME /////
// ////////////////////////////

// Set the clock label font and text size
void set_clock_label(GtkWidget *clock, char *fmt, const char *font, int fsize) {

    // Obtain current time as seconds elapsed since the Epoch.
    time_t current_time = time(NULL);
    char time_string[50];
    struct tm *tmp;
    
    // Convert time
    tmp = localtime(&current_time);
    strftime(time_string, sizeof(time_string), fmt, tmp);
    
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(font);
    PangoAttribute *attrSize = pango_attr_size_new(fsize);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrFont);
    
    
    // Set the label text and font
    gtk_label_set_attributes(GTK_LABEL(clock), attrList);
    gtk_label_set_text(GTK_LABEL(clock), time_string);
}



// ////////////////////////
// ///// UPDATE CLOCK /////
// ////////////////////////

// Refresh the current clock label
gboolean clock_date_update(gpointer data) {
    GtkWidget *clock = (GtkWidget *) data;
    set_clock_label(clock, CLOCK_DATE_FMT, CLOCK_DATE_FONT, CLOCK_DATE_FSIZE);
    
    return TRUE;
}


// Refresh the current clock label
gboolean clock_time_update(gpointer data) {
    GtkWidget *clock = (GtkWidget *) data;
    set_clock_label(clock, CLOCK_TIME_FMT, CLOCK_TIME_FONT, CLOCK_TIME_FSIZE);
    
    return TRUE;
}



// /////////////////////////
// ///// DISPLAY CLOCK /////
// /////////////////////////

// Display the date and time clock
void display_clock() {
    
    // Initialize clock elements
    GtkWidget *date_clock_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *date_clock = gtk_label_new("");
    GtkWidget *time_clock_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *time_clock = gtk_label_new("");
    
    // Setup date and time clocks 
    init_clock_date_root(date_clock_window, date_clock);
    init_clock_time_root(time_clock_window, time_clock);
    
    // Display the clock
    gtk_widget_show(date_clock);
    gtk_widget_show(date_clock_window);
    gtk_widget_show(time_clock);
    gtk_widget_show(time_clock_window);
}
