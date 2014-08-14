// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Panel.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the following:
// 
//         #include "../hdr/Panel.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o Panel Panel.c Transparency.c `pkg-config gtk+-3.0 gdk-3.0 --cflags --libs` 
//         $ ./Power
// 
// 
// PURPOSE:
// 
//     Display buttons that control shutting down and rebooting the system.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     init_shutdown_root      - Initialize shutdown button
//     init_reboot_root        - Initialize reboot button
//     init_refresh_login_root - Initialize refresh login button
// 
//     system_shutdown         - Command to shutdown computer
//     system_reboot           - Command to reboot computer
//     refresh_login           - Command to refresh the login manager
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Panel Buttons 
//     * System Commands
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 11 2014 <> created
// 
//     gabeg Aug 12 2014 <> Changed name from 'Power.c' to 'Panel.c' and added
//                          a Refresh Login Screen function
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Panel.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>

#define   SHUT_XPOS       gdk_screen_width()-32*1
#define   SHUT_YPOS       gdk_screen_height()-32
#define   SHUT_WIDTH      32
#define   SHUT_HEIGHT     32
#define   SHUT_IMG_FILE   "/etc/X11/glm/img/interface/shutdown.png"
#define   SHUT_COM        "/usr/bin/poweroff"

#define   REB_XPOS       gdk_screen_width()-32*2
#define   REB_YPOS       gdk_screen_height()-32
#define   REB_WIDTH      32
#define   REB_HEIGHT     32
#define   REB_IMG_FILE   "/etc/X11/glm/img/interface/reboot.png"
#define   REB_COM        "/usr/bin/reboot"

#define   LREF_XPOS       gdk_screen_width()-32*3
#define   LREF_YPOS       gdk_screen_height()-32
#define   LREF_WIDTH      32
#define   LREF_HEIGHT     32
#define   LREF_IMG_FILE   "/etc/X11/glm/img/interface/refresh.png"
#define   LREF_COM        "/usr/bin/systemctl"


// Declares
void init_shutdown_root(GtkWidget *shutdow_window, GtkWidget *shutdown);
void init_reboot_root(GtkWidget *reboot_window, GtkWidget *reboot);
void init_refresh_login_root(GtkWidget *refresh_window, GtkWidget *refresh);
void system_shutdown();
void system_reboot();
void refresh_login();



// ////////////////////////////////////
// ///// INITIALIZE PANEL BUTTONS /////
// ////////////////////////////////////

// Initialize the shutdown root window
void init_shutdown_root(GtkWidget *shutdown_window, GtkWidget *shutdown) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(shutdown_window), SHUT_XPOS, SHUT_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(shutdown_window), SHUT_WIDTH*0, SHUT_HEIGHT*0);
    
    // Set power button attributes
    const GdkRGBA bg_widget = {0, 0, 0, 0};
    const GdkRGBA fg_widget = {1, 1, 1, 1};
    set_color_and_opacity(shutdown_window, shutdown, bg_widget, fg_widget);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(SHUT_IMG_FILE);
    gtk_button_set_image(GTK_BUTTON(shutdown), image);
    gtk_button_set_relief(GTK_BUTTON(shutdown), GTK_RELIEF_NONE);
    gtk_window_set_decorated(GTK_WINDOW(shutdown_window), FALSE);
    
    // Add the clock to the root window 
    gtk_container_add(GTK_CONTAINER(shutdown_window), shutdown);
        
    // Enable transparency
    enable_transparency(shutdown_window);
    
    // GTK signal
    g_signal_connect(G_OBJECT(shutdown), "clicked", G_CALLBACK(system_shutdown), NULL);
    g_signal_connect(shutdown_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// Initialize the shutdown root window
void init_reboot_root(GtkWidget *reboot_window, GtkWidget *reboot) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(reboot_window), REB_XPOS, REB_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(reboot_window), REB_WIDTH*0, REB_HEIGHT*0);
    
    // Set power button attributes
    const GdkRGBA bg_widget = {0, 0, 0, 0};
    const GdkRGBA fg_widget = {1, 1, 1, 1};
    set_color_and_opacity(reboot_window, reboot, bg_widget, fg_widget);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(REB_IMG_FILE);
    gtk_button_set_image(GTK_BUTTON(reboot), image);
    gtk_button_set_relief(GTK_BUTTON(reboot), GTK_RELIEF_NONE);
    gtk_window_set_decorated(GTK_WINDOW(reboot_window), FALSE);
    
    // Add the clock to the root window 
    gtk_container_add(GTK_CONTAINER(reboot_window), reboot);
        
    // Enable transparency
    enable_transparency(reboot_window);
    
    // GTK signal
    g_signal_connect(G_OBJECT(reboot), "clicked", G_CALLBACK(system_reboot), NULL);
    g_signal_connect(reboot_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// Initialize the refresh login root window
void init_refresh_login_root(GtkWidget *refresh_window, GtkWidget *refresh) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(refresh_window), LREF_XPOS, LREF_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(refresh_window), LREF_WIDTH*0, LREF_HEIGHT*0);
    
    // Set power button attributes
    const GdkRGBA bg_widget = {0, 0, 0, 0};
    const GdkRGBA fg_widget = {1, 1, 1, 1};
    set_color_and_opacity(refresh_window, refresh, bg_widget, fg_widget);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(LREF_IMG_FILE);
    gtk_button_set_image(GTK_BUTTON(refresh), image);
    gtk_button_set_relief(GTK_BUTTON(refresh), GTK_RELIEF_NONE);
    gtk_window_set_decorated(GTK_WINDOW(refresh_window), FALSE);
    
    // Add the clock to the root window 
    gtk_container_add(GTK_CONTAINER(refresh_window), refresh);
        
    // Enable transparency
    enable_transparency(refresh_window);
    
    // GTK signal
    g_signal_connect(G_OBJECT(refresh), "clicked", G_CALLBACK(refresh_login), NULL);
    g_signal_connect(refresh_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// ///////////////////////////
// ///// SYSTEM COMMANDS /////
// ///////////////////////////

// Shutdown the system
void system_shutdown() {
    execl(SHUT_COM, SHUT_COM, NULL);
}



// Shutdown the system
void system_reboot() {
    execl(REB_COM, REB_COM, NULL);
}



// Refresh the login screen
void refresh_login() {
    execl(LREF_COM, LREF_COM, "restart", "glm.service", NULL);
}
