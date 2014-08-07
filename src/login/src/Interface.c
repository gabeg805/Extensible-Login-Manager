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
    
    // Define clock 
    GtkWidget *clock_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *clock = gtk_label_new("");
        
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
    init_clock_root(clock_window, clock);
    init_frame_root(frame_window, frame);
    init_text_root(text_window, text);
    init_wm_root(wm_window, wm_dropmenu, wm_menu);    
        
        
    // Display the clock
    gtk_widget_show(clock);
    gtk_widget_show(clock_window);
    g_timeout_add_seconds(1, clock_update, clock);
        
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
