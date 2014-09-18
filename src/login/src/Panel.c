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
//     Include the following:
// 
//         #include "../hdr/Panel.h"
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
//     display_panel           - Display button panel
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Panel Buttons 
//     * System Commands
//     * Display Button Panel
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 11 2014 <> created
// 
//     gabeg Aug 12 2014 <> Changed name from 'Power.c' to 'Panel.c' and added
//                          a Refresh Login Screen function
// 
//     gabeg Aug 20 2014 <> Moved the code inside Interface.c that displays the 
//                          button panel into the main Panel.c module 
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Panel.h"
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <stdlib.h>


// Declares
void init_shutdown_root(GtkWidget *shutdow_window, GtkWidget *shutdown);
void init_reboot_root(GtkWidget *reboot_window, GtkWidget *reboot);
void init_glm_dialog_root(GtkWidget *refresh_window, GtkWidget *refresh);
void init_glm_dialog();
void system_shutdown();
void system_reboot();
void cancel_glm();
void refresh_glm();
void quit_glm();
void display_panel();



// ////////////////////////////////////
// ///// INITIALIZE PANEL BUTTONS /////
// ////////////////////////////////////

// Initialize the shutdown root window
void init_shutdown_root(GtkWidget *shutdown_window, GtkWidget *shutdown) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(shutdown_window), PANEL_SHUTDOWN_XPOS, PANEL_SHUTDOWN_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(shutdown_window), PANEL_SHUTDOWN_WIDTH*0, PANEL_SHUTDOWN_HEIGHT*0);
    
    // Set power button attributes
    set_color_and_opacity(shutdown_window, shutdown, BG_SHUTDOWN, FG_SHUTDOWN);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(PANEL_SHUTDOWN_IMG);
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
    gtk_window_move(GTK_WINDOW(reboot_window), PANEL_REBOOT_XPOS, PANEL_REBOOT_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(reboot_window), PANEL_REBOOT_WIDTH*0, PANEL_REBOOT_HEIGHT*0);
    
    // Set power button attributes
    set_color_and_opacity(reboot_window, reboot, BG_REBOOT, FG_REBOOT);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(PANEL_REBOOT_IMG);
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
void init_glm_dialog_root(GtkWidget *refresh_window, GtkWidget *refresh) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(refresh_window), PANEL_REFRESH_XPOS, PANEL_REFRESH_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(refresh_window), PANEL_REFRESH_WIDTH*0, PANEL_REFRESH_HEIGHT*0);
    
    // Set power button attributes
    set_color_and_opacity(refresh_window, refresh, BG_GLM_DIALOG, FG_GLM_DIALOG);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(PANEL_REFRESH_IMG);
    gtk_button_set_image(GTK_BUTTON(refresh), image);
    gtk_button_set_relief(GTK_BUTTON(refresh), GTK_RELIEF_NONE);
    gtk_window_set_decorated(GTK_WINDOW(refresh_window), FALSE);
    
    // Add the clock to the root window 
    gtk_container_add(GTK_CONTAINER(refresh_window), refresh);
        
    // Enable transparency
    enable_transparency(refresh_window);
    
    // GTK signal
    g_signal_connect(G_OBJECT(refresh), "clicked", G_CALLBACK(init_glm_dialog), NULL);
    g_signal_connect(refresh_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// Initialize the login prompt
void init_glm_dialog() {
    
    // Initialize widgets
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *label = gtk_label_new("Choose an action for Gabe's Login Manager.");
    GtkWidget *blank = gtk_label_new(" ");

    GtkWidget *refresh_button = gtk_button_new_with_label("Restart");
    GtkWidget *quit_button    = gtk_button_new_with_label("Quit");
    GtkWidget *cancel_button  = gtk_button_new_with_label("Cancel");
    
    // Grid attributes
    gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
    
    // Place widgets on the grid
    gtk_grid_attach(GTK_GRID(grid), label,            0, 0, 5, 1);
    gtk_grid_attach(GTK_GRID(grid), blank,            0, 1, 5, 1);
    gtk_grid_attach(GTK_GRID(grid), refresh_button,   0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cancel_button,    3, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), quit_button,      4, 2, 1, 1);
    
    // Add grid to window
    gtk_window_move(GTK_WINDOW(window), PANEL_DIALOG_XPOS, PANEL_DIALOG_YPOS);
    gtk_container_add(GTK_CONTAINER(window), grid);
    
    // Show widgets
    gtk_widget_show(label);
    gtk_widget_show(blank);    
    gtk_widget_show(refresh_button);
    gtk_widget_show(quit_button);
    gtk_widget_show(cancel_button);
    gtk_widget_show(grid);
    gtk_widget_show(window);
    
    // GTK signals
    g_signal_connect(G_OBJECT(refresh_button), "clicked", G_CALLBACK(refresh_glm), NULL);
    g_signal_connect(G_OBJECT(quit_button), "clicked", G_CALLBACK(quit_glm), NULL);
    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(cancel_glm), window);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// ///////////////////////////
// ///// SYSTEM COMMANDS /////
// ///////////////////////////

// Shutdown the system
void system_shutdown() {
    execl(POWEROFF, POWEROFF, NULL);
}



// Shutdown the system
void system_reboot() {
    execl(REBOOT, REBOOT, NULL);
}



// Quit the prompt
void cancel_glm(GtkWidget *button, GtkWidget *window) {
    gtk_widget_destroy(window);
}



// Refresh the login screen
void refresh_glm() {
    execl(SYSTEMCTL, SYSTEMCTL, "restart", SERVICE, NULL);
}



// Quit the login screen
void quit_glm() {
    char *cmd = "/usr/bin/chvt 2; /usr/bin/systemctl stop glm";
    execl("/bin/bash", "/bin/bash", "-c", cmd, NULL);
}



// ////////////////////////////////
// ///// DISPLAY BUTTON PANEL /////
// ////////////////////////////////

// Display button panel 
void display_panel() {
    
    // Initialize button panel items
    GtkWidget *shutdown_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *shutdown = gtk_button_new();
    GtkWidget *reboot_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *reboot = gtk_button_new();
    GtkWidget *refresh_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *refresh = gtk_button_new();
    
    // Setup buttons on the panel
    init_shutdown_root(shutdown_window, shutdown);
    init_reboot_root(reboot_window, reboot);
    init_glm_dialog_root(refresh_window, refresh);
    
    // Display the button panel
    gtk_widget_show(shutdown);
    gtk_widget_show(shutdown_window);
    
    gtk_widget_show(reboot);
    gtk_widget_show(reboot_window);
    
    gtk_widget_show(refresh);
    gtk_widget_show(refresh_window);
}
