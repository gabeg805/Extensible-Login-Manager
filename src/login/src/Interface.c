// 
// Created By: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// Name:
// 
//     Interface.c
// 
// 
// Syntax: 
// 
//     ./Interface
// 
// 
// Purpose:
// 
//     Display the login interface.
// 
// 
// Keywords:
// 
//     Unknown
// 
// 
// Functions:
// 
//     login_interface - Display the login interface
// 
// 
// File Structure:
// 
//     * Includes and Declares
//     * Display Interface
// 
// 
// Modification History:
// 	
//     gabeg Aug 07 2014 <> created
// 
// **********************************************************************************


// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Interface.h"
#include "../hdr/Transparency.h"
#include "../hdr/Clock.h"
#include "../hdr/Frame.h"
#include "../hdr/TextImage.h"
#include "../hdr/WindowManager.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#define log "/etc/X11/glm/log/interface.log"


// Declares
void login_interface(int argc, char *argv[]);



// /////////////////////////////
// ///// DISPLAY INTERFACE ///// 
// /////////////////////////////

// Display the login interface
void login_interface(int argc, char *argv[]) {
    
    // Initialize GTK toolkit
    gtk_init(&argc, &argv);
    
    // Define clock date
    GtkWidget *clock_date_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *clock_date = gtk_label_new("");
    
    // Define clock time
    GtkWidget *clock_time_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *clock_time = gtk_label_new("");
    
    // Define frame  
    GtkWidget *frame_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *frame = gtk_drawing_area_new();
    
    // Define text image
    GtkWidget *text_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *text = gtk_drawing_area_new();
    
    // Define window manager
    GtkWidget *wm_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *wm_dropmenu = gtk_menu_button_new();
    GtkWidget *wm_menu = gtk_menu_new();    
    
    // Initialize the root window with all its objects
    init_date_clock_root(clock_date_window, clock_date);
    init_time_clock_root(clock_time_window, clock_time);
    init_frame_root(frame_window, frame);
    init_text_root(text_window, text);
    init_wm_root(wm_window, wm_dropmenu, wm_menu);    
    
    
    // Display the clock date
    gtk_widget_show(clock_date);
    gtk_widget_show(clock_date_window);
    
    // Display the clock time
    gtk_widget_show(clock_time);
    gtk_widget_show(clock_time_window);
    
    // Display the login frame
    gtk_widget_show(frame);
    gtk_widget_show(frame_window);
    
    // Display the password text image
    gtk_widget_show(text);
    gtk_widget_show(text_window);
    
    // Display the dropdown menu
    set_wm_entries(wm_menu);
    gtk_widget_show(wm_dropmenu);
    gtk_widget_show(wm_window);
    
    
    // Log to file that interface is beginning execution
    FILE *fp;
    fp = fopen(log, "w");
    fprintf(fp, "%s\n", "TRUE");
    fclose(fp);
    
    gtk_main();
}
