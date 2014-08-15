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
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Date and Time Clock
//     * Set Current Time
//     * Update the Clock
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 02 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Clock.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <time.h>

#define   CLOCK_DATE_XPOS     50
#define   CLOCK_DATE_YPOS     650
#define   CLOCK_DATE_WIDTH    400
#define   CLOCK_DATE_HEIGHT   50
#define   CLOCK_DATE_FONT     "Inconsolata"
#define   CLOCK_DATE_FSIZE    25*1024
#define   CLOCK_DATE_FMT      "%A, %B %-d"

#define   CLOCK_TIME_XPOS     86
#define   CLOCK_TIME_YPOS     575
#define   CLOCK_TIME_WIDTH    400
#define   CLOCK_TIME_HEIGHT   75
#define   CLOCK_TIME_FONT     "Inconsolata"
#define   CLOCK_TIME_FSIZE    45*1024
#define   CLOCK_TIME_FMT      "%-I:%M %p"

#define   CLOCK_UPDATE_SEC   30

// Declares
void init_clock_date_root(GtkWidget *window, GtkWidget *clock);
void init_clock_time_root(GtkWidget *window, GtkWidget *clock);
void set_clock_label(GtkWidget *clock, char *fmt, const char *font, int fsize);
gboolean clock_date_update(gpointer data);
gboolean clock_time_update(gpointer data);



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



// ////////////////////////////
// ///// UPDATE THE CLOCK /////
// ////////////////////////////

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
