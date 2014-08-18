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
//         $ gcc -o Interface Interface.c Username.c Password.c Clock.c Frame.c TextImage.c WindowManager.c Panel.c Transparency.c FileRW.c `pkg-config gtk+-3.0 --cflags --libs`
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
//     N/A
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
//     gabeg Aug 14 2014 <> Updated gcc command to execute in the header to include
//                          FileRW.c and included Password.c into the main loop
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Interface.h"
#include "../hdr/Username.h"
#include "../hdr/Password.h"
#include "../hdr/Clock.h"
#include "../hdr/Frame.h"
#include "../hdr/TextImage.h"
#include "../hdr/WindowManager.h"
#include "../hdr/Panel.h"
#include "../hdr/Transparency.h"
#include "../hdr/FileRW.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define INTERFACE_LOG_FILE   "/etc/X11/glm/log/interface.log"
#define INTERFACE_FLAG       "TRUE"


// Declares
char * login_interface(int argc, char *argv[]);

char output[100];


// ///////////////////////////////////
// ///// DISPLAY LOGIN INTERFACE ///// 
// ///////////////////////////////////

// Display the login interface
char * login_interface(int argc, char *argv[]) {
    
    // Initialize GTK toolkit
    gtk_init(&argc, &argv);

    // Check file to see if interface requires setup
    char *flag = file_read(INTERFACE_LOG_FILE);
    
    if ( strcmp(flag, INTERFACE_FLAG) == 0 ) {
        
        // Define the clock
        GtkWidget *date_clock_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *date_clock = gtk_label_new("");
        GtkWidget *time_clock_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *time_clock = gtk_label_new("");
        
        // Define the frame
        GtkWidget *frame_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *frame = gtk_drawing_area_new();
        
        // Define the text image
        GtkWidget *text_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *text = gtk_drawing_area_new();
        
        // Define the window manager
        GtkWidget *wm_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *wm_dropmenu = gtk_menu_button_new();
        GtkWidget *wm_menu = gtk_menu_new();
        
        // Define the username menu
        GtkWidget *user_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *user_dropmenu = gtk_menu_button_new();
        GtkWidget *user_menu = gtk_menu_new();
        GtkWidget *user_label = gtk_label_new("");
        
        // Define the power button window
        GtkWidget *shutdown_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *shutdown = gtk_button_new();
        GtkWidget *reboot_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *reboot = gtk_button_new();
        GtkWidget *refresh_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        GtkWidget *refresh = gtk_button_new();
        
        
        // Initialize the root window with all its objects
        init_clock_date_root(date_clock_window, date_clock);
        init_clock_time_root(time_clock_window, time_clock);
        init_frame_root(frame_window, frame);
        init_text_root(text_window, text);
        init_wm_root(wm_window, wm_dropmenu, wm_menu);
        init_usermenu_root(user_window, user_dropmenu, user_menu, user_label);
        init_shutdown_root(shutdown_window, shutdown);
        init_reboot_root(reboot_window, reboot);
        init_glm_dialog_root(refresh_window, refresh);
        
        
        // Display the clock
        gtk_widget_show(date_clock);
        gtk_widget_show(date_clock_window);
        gtk_widget_show(time_clock);
        gtk_widget_show(time_clock_window);
        
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
        
        // Display the power button
        gtk_widget_show(shutdown);
        gtk_widget_show(shutdown_window);
        gtk_widget_show(reboot);
        gtk_widget_show(reboot_window);
        gtk_widget_show(refresh);
        gtk_widget_show(refresh_window);

        // Log that interface has already begun
        file_write(INTERFACE_LOG_FILE, "FALSE", "%s\n");
    }
    
    
    // Display entry box
    GtkWidget *password_entry_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *password_entry = gtk_entry_new();
    init_entry(password_entry);
    init_entry_root(password_entry_window, password_entry);
    gtk_widget_show(password_entry);
    gtk_widget_show(password_entry_window);
    
    
    // Begin GTK main loop
    gtk_main();
    
    
    // Allocate memory for password output 
    size_t sz = strlen(output);
    char *pass = malloc(sz+1);
    snprintf(pass, sz+1, output);
    
    free(flag);
    
    return pass;
}
