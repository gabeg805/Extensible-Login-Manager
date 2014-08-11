// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Power.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the following:
// 
//         #include "../hdr/Power.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o Power Power.c Transparency.c `pkg-config gtk+-3.0 gdk-3.0 --cflags --libs` 
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
//     main - the main function
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Objective
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg MON DAY YEAR <> created
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Power.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>

#define   SHUT_XPOS       gdk_screen_width()-32
#define   SHUT_YPOS       gdk_screen_height()-32
#define   SHUT_WIDTH      32
#define   SHUT_HEIGHT     32
#define   SHUT_IMG_FILE   "/etc/X11/glm/img/interface/shutdown.png"
#define   SHUT_COM        "/usr/bin/poweroff"

#define   REB_XPOS       gdk_screen_width()-64
#define   REB_YPOS       gdk_screen_height()-32
#define   REB_WIDTH      32
#define   REB_HEIGHT     32
#define   REB_IMG_FILE   "/etc/X11/glm/img/interface/reboot.png"
#define   REB_COM        "/usr/bin/reboot"


// Declares
void init_shutdown_root(GtkWidget *shutdow_window, GtkWidget *shutdown);
void init_reboot_root(GtkWidget *reboot_window, GtkWidget *reboot);
void set_power_color(GtkWidget *window, GtkWidget *power);
void system_shutdown();
void system_reboot();



// //////////////////////////////////////////
// ///// INITIALIZE POWER BUTTON WINDOW /////
// //////////////////////////////////////////

// Initialize the shutdown root window
void init_shutdown_root(GtkWidget *shutdown_window, GtkWidget *shutdown) {
    
    // Set window attributes
    gtk_window_set_title(GTK_WINDOW(shutdown_window), "Shutdown System Button");
    gtk_window_move(GTK_WINDOW(shutdown_window), SHUT_XPOS, SHUT_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(shutdown_window), SHUT_WIDTH*0, SHUT_HEIGHT*0);
    
    // Set power button attributes
    set_power_color(shutdown_window, shutdown);
    
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
    gtk_window_set_title(GTK_WINDOW(reboot_window), "Reboot System Button");
    gtk_window_move(GTK_WINDOW(reboot_window), REB_XPOS, REB_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(reboot_window), REB_WIDTH*0, REB_HEIGHT*0);
    
    // Set power button attributes
    set_power_color(reboot_window, reboot);
    
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



// //////////////////////////////////
// ///// SET POWER BUTTON COLOR /////
// //////////////////////////////////

// Set color of power button window
void set_power_color(GtkWidget *window, GtkWidget *power) {
    
    // Define the color scheme
    const GdkRGBA bg_window = {0, 0, 0, 0};
    const GdkRGBA fg_window = {0, 0, 0, 0};
    const GdkRGBA bg_power = {0, 0, 0, 0};
    const GdkRGBA fg_power = {1, 1, 1, 1};
    
    // Set the color scheme
    gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &bg_window);
    gtk_widget_override_background_color(power, GTK_STATE_FLAG_NORMAL, &bg_power);
    gtk_widget_override_color(window, GTK_STATE_FLAG_NORMAL, &fg_window);
    gtk_widget_override_color(power, GTK_STATE_FLAG_NORMAL, &fg_power);    
}



// /////////////////////////////////
// ///// SYSTEM POWER COMMANDS /////
// /////////////////////////////////

// Shutdown the system
void system_shutdown() {
    execl(SHUT_COM, SHUT_COM, NULL);
}



// Shutdown the system
void system_reboot() {
    execl(REB_COM, REB_COM, NULL);
}



/* // Main function */
/* void main(int argc, char *argv[]) { */
    
/*     // Initialize GTK toolkit */
/*     gtk_init(&argc, &argv); */
    
/*     // Define the power button window */
/*     GtkWidget *shutdown_window = gtk_window_new(GTK_WINDOW_TOPLEVEL); */
/*     GtkWidget *shutdown = gtk_button_new(); */
    
/*     GtkWidget *reboot_window = gtk_window_new(GTK_WINDOW_TOPLEVEL); */
/*     GtkWidget *reboot = gtk_button_new(); */
    
    
/*     // Initialize the power button window */
/*     init_shutdown_root(shutdown_window, shutdown); */
/*     init_reboot_root(reboot_window, reboot); */
    
/*     // Display the power button window */
/*     gtk_widget_show(shutdown); */
/*     gtk_widget_show(shutdown_window); */
    
/*     gtk_widget_show(reboot); */
/*     gtk_widget_show(reboot_window); */

/*     gtk_main(); */

/* } */
