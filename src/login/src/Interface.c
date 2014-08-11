// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Interface.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/Interface.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o Interface Interface.c \
//               Username.c Transparency.c Clock.c Frame.c TextImage.c WindowManager.c \
//               `pkg-config gtk+-3.0 --cflags --libs`
//         $ ./Interface
// 
// 
// PURPOSE:
// 
//     Display the login interface.
// 
// 
// KEYWORDS:
// 
//     Unknown
// 
// 
// FUNCTIONS:
// 
//     login_interface - Display the login interface
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Display Login Interface
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 07 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Interface.h"
#include "../hdr/Username.h"
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



// ///////////////////////////////////
// ///// DISPLAY LOGIN INTERFACE ///// 
// ///////////////////////////////////

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
    
    // Define username menu
    GtkWidget *user_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *user_dropmenu = gtk_menu_button_new();
    GtkWidget *user_menu = gtk_menu_new();
    GtkWidget *user_label = gtk_label_new("");
    
    
    // Initialize the root window with all its objects
    init_date_clock_root(clock_date_window, clock_date);
    init_time_clock_root(clock_time_window, clock_time);
    init_frame_root(frame_window, frame);
    init_text_root(text_window, text);
    init_wm_root(wm_window, wm_dropmenu, wm_menu);    
    init_usermenu_root(user_window, user_dropmenu, user_menu, user_label);    
    
    
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
    
    // Display the username menu
    set_username_entries(user_menu, user_label);
    gtk_widget_show(user_label);
    gtk_widget_show(user_dropmenu);
    gtk_widget_show(user_window);
    
        
    // Log to file that interface is beginning execution
    FILE *fp;
    fp = fopen(log, "w");
    fprintf(fp, "%s\n", "TRUE");
    fclose(fp);
    
    gtk_main();
}
