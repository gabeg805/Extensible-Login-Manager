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
//     init_clock      - Setup the clock window and widget 
// 
//     set_clock_label - Set the clock label (font, text size, and format)
// 
//     clock_update    - Refresh the clock label
// 
//     display_clock   - Display the clock
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Setup Clock
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
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Clock.h"
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"
#include "../hdr/Utility.h"

#include <gtk/gtk.h>
#include <time.h>
#include <stdlib.h>

#define CLOCK_DATE_FMT     "%A, %B %-d"
#define CLOCK_UPDATE_SEC   30
#define CLOCK_TIME_FMT     "%-I:%M %p"
#define CLOCK_DATE_XPOS    50
#define CLOCK_TIME_XPOS    86
#define CLOCK_DATE_YPOS    650
#define CLOCK_TIME_YPOS    575
#define CLOCK_DATE_WIDTH   400
#define CLOCK_TIME_WIDTH   400
#define CLOCK_DATE_HEIGHT  50
#define CLOCK_TIME_HEIGHT  75
#define BG_DATE_CLOCK      (const GdkRGBA) {0, 0, 0, 0}
#define BG_TIME_CLOCK      (const GdkRGBA) {0, 0, 0, 0}
#define FG_DATE_CLOCK      (const GdkRGBA) {1, 1, 1, 1}
#define FG_TIME_CLOCK      (const GdkRGBA) {1, 1, 1, 1}
#define BG_WINDOW          (const GdkRGBA) {0, 0, 0, 0}
#define FG_WINDOW          (const GdkRGBA) {0, 0, 0, 0}
#define CLOCK_DATE_FSIZE   25*1024
#define CLOCK_TIME_FSIZE   45*1024
#define CLOCK_DATE_FONT    "Inconsolata"
#define CLOCK_TIME_FONT    "Inconsolata"


// Declares
void setup(struct glmgui *gui, struct glmtext *text);
void set_label(struct glmtext *text);
gboolean update(gpointer data);
void display_clock();



// ///////////////////////
// ///// SETUP CLOCK /////
// ///////////////////////

// Setup the window and widget
void setup(struct glmgui *gui, struct glmtext *text) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(gui->win), gui->x, gui->y);
    gtk_window_set_default_size(GTK_WINDOW(gui->win), gui->width, gui->height);
    
    // Set clock attributes
    set_stuff(gui);
    set_label(text);
    
    // Add the clock to the root window 
    gtk_container_add(GTK_CONTAINER(gui->win), gui->widg);
    g_timeout_add_seconds(CLOCK_UPDATE_SEC, update, text);
    
    // Enable transparency
    enable_transparency(gui->win);
    
    // GTK signal
    g_signal_connect(gui->win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// //////////////////////////
// ///// SET CLOCK TIME /////
// //////////////////////////

// Set the clock label font and text size
void set_label(struct glmtext *text) {

    // Obtain current time as seconds elapsed since the Epoch.
    time_t current_time = time(NULL);
    char time_string[50];
    struct tm *tmp;
    
    // Convert time
    tmp = localtime(&current_time);
    strftime(time_string, sizeof(time_string), text->fmt, tmp);
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(text->font);
    PangoAttribute *attrSize = pango_attr_size_new(text->size);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrFont);
    
    // Set the label text and font
    gtk_label_set_attributes(GTK_LABEL(text->widget), attrList);
    gtk_label_set_text(GTK_LABEL(text->widget), time_string);
}



// ////////////////////////
// ///// UPDATE CLOCK /////
// ////////////////////////

// Refresh the current clock label
gboolean update(gpointer data) {
    struct glmtext *text = (struct glmtext *) data;
    set_label(text);
    
    return TRUE;
}



// /////////////////////////
// ///// DISPLAY CLOCK /////
// /////////////////////////

// Display the date and time clock
void display_clock() {
    
    // Initialize date gui widget
    struct glmgui *date_gui = setup_gui_struct(gtk_window_new(GTK_WINDOW_TOPLEVEL), 
                                               gtk_label_new(""), 
                                               BG_WINDOW, FG_WINDOW, 
                                               BG_DATE_CLOCK, FG_DATE_CLOCK, 
                                               CLOCK_DATE_XPOS, CLOCK_DATE_YPOS, 
                                               CLOCK_DATE_WIDTH*0, CLOCK_DATE_HEIGHT*0);
    
    // Initialize time gui widget
    struct glmgui *time_gui = setup_gui_struct(gtk_window_new(GTK_WINDOW_TOPLEVEL), 
                                               gtk_label_new(""), 
                                               BG_WINDOW, FG_WINDOW, 
                                               BG_TIME_CLOCK, FG_TIME_CLOCK, 
                                               CLOCK_TIME_XPOS, CLOCK_TIME_YPOS, 
                                               CLOCK_TIME_WIDTH*0, CLOCK_TIME_HEIGHT*0);
    
    // Initialize date text attributes
    struct glmtext *date_text = setup_text_struct(date_gui->widg, CLOCK_DATE_FONT, 
                                                  CLOCK_DATE_FSIZE, CLOCK_DATE_FMT);
    
    // Initialize time text attributes
    struct glmtext *time_text = setup_text_struct(time_gui->widg, CLOCK_TIME_FONT, 
                                                  CLOCK_TIME_FSIZE, CLOCK_TIME_FMT);
    
    // Setup date and time clocks 
    setup(date_gui, date_text);
    setup(time_gui, time_text);
    
    // Display the clock
    gtk_widget_show(date_gui->widg);
    gtk_widget_show(date_gui->win);
    gtk_widget_show(time_gui->widg);
    gtk_widget_show(time_gui->win);
}
