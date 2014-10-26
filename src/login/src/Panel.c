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
//     setup_button    - Initialize refresh login button
//     setup_dialog    - Initialize refresh login button
// 
//     system_shutdown - Command to shutdown computer
//     system_reboot   - Command to reboot computer
//     cancel_glm      - Command to close the refresh login popup
//     refresh_glm     - Command to refresh the login manager
//     quit_glm        - Command to quit the login manager
// 
//     display_panel   - Display all buttons on the panel
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Setup Panel Buttons 
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
//     gabeg Oct 25 2014 <> Removed structs that were sucking up a lot of memory. 
//                          Added a setup function for the panel items, and removed 
//                          the general setup function that was written. Instead of 
//                          the general setup function, I used the universal setup 
//                          function.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"
#include "../hdr/Panel.h"
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <stdlib.h>

#define SHUTDOWN_XPOS     1366-32*1
#define REBOOT_XPOS       1366-32*2
#define REFRESH_XPOS      1366-32*3
#define DIALOG_XPOS       505
#define SHUTDOWN_YPOS     768-32
#define REBOOT_YPOS       768-32
#define REFRESH_YPOS      768-32
#define DIALOG_YPOS       310
#define SHUTDOWN_WIDTH    0
#define REBOOT_WIDTH      0
#define REFRESH_WIDTH     0
#define SHUTDOWN_HEIGHT   0
#define REBOOT_HEIGHT     0
#define REFRESH_HEIGHT    0
#define BG_SHUTDOWN       (const GdkRGBA) {0, 0, 0, 0}
#define BG_REBOOT         (const GdkRGBA) {0, 0, 0, 0}
#define BG_REFRESH        (const GdkRGBA) {0, 0, 0, 0}
#define BG_WIN            (const GdkRGBA) {0, 0, 0, 0}
#define FG_WIN            (const GdkRGBA) {0, 0, 0, 0}
#define FG_SHUTDOWN       (const GdkRGBA) {1, 1, 1, 1}
#define FG_REBOOT         (const GdkRGBA) {1, 1, 1, 1}
#define FG_REFRESH        (const GdkRGBA) {1, 1, 1, 1}
#define SHUTDOWN_IMG      "/etc/X11/glm/img/interface/shutdown.png"
#define REBOOT_IMG        "/etc/X11/glm/img/interface/reboot.png"
#define REFRESH_IMG       "/etc/X11/glm/img/interface/refresh.png"
#define POWEROFF          "/usr/bin/emacs"
#define REBOOT            "/usr/bin/urxvt"
#define SYSTEMCTL         "/usr/bin/systemctl"


// Declares
static void setup_button(GtkWidget *widg, char *pic);
static void setup_dialog();
static void system_shutdown();
static void system_reboot();
static void cancel_glm();
static void refresh_glm();
static void quit_glm();
void display_panel();



// ///////////////////////////////
// ///// SETUP PANEL BUTTONS /////
// ///////////////////////////////

// Setup panel buttons
static void setup_button(GtkWidget *widg, char *pic) {
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(pic);
    gtk_button_set_image(GTK_BUTTON(widg), image);
    gtk_button_set_relief(GTK_BUTTON(widg), GTK_RELIEF_NONE);
}



// Setup the dialog for the buttons
static void setup_dialog() {
    
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
    gtk_window_move(GTK_WINDOW(window), DIALOG_XPOS, DIALOG_YPOS);
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
static void system_shutdown() {
    execl(POWEROFF, POWEROFF, NULL);
}



// Shutdown the system
static void system_reboot() {
    execl(REBOOT, REBOOT, NULL);
}



// Quit the prompt
static void cancel_glm(GtkWidget *button, GtkWidget *window) {
    gtk_widget_hide(window);
}



// Refresh the login screen
static void refresh_glm() {
    execl(SYSTEMCTL, SYSTEMCTL, "restart", SERVICE, NULL);
}



// Quit the login screen
static void quit_glm() {
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
    GtkWidget *reboot_window   = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *refresh_window  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *shutdown = gtk_button_new();
    GtkWidget *reboot   = gtk_button_new();
    GtkWidget *refresh  = gtk_button_new();
    
    // Define structs to hold widget information
    int shutdown_pos[4] = {SHUTDOWN_XPOS, SHUTDOWN_YPOS, SHUTDOWN_WIDTH, SHUTDOWN_HEIGHT};
    int reboot_pos[4] = {REBOOT_XPOS, REBOOT_YPOS, REBOOT_WIDTH, REBOOT_HEIGHT};
    int refresh_pos[4] = {REFRESH_XPOS, REFRESH_YPOS, REFRESH_WIDTH, REFRESH_HEIGHT};
    
    const GdkRGBA shutdown_color[4] = {BG_WIN, BG_SHUTDOWN, FG_WIN, FG_SHUTDOWN};
    const GdkRGBA reboot_color[4] = {BG_WIN, BG_REBOOT, FG_WIN, FG_REBOOT};
    const GdkRGBA refresh_color[4] = {BG_WIN, BG_REFRESH, FG_WIN, FG_REFRESH};
    
    
    // Setup widgets
    setup_widget(shutdown_window, shutdown, shutdown_pos, shutdown_color);
    setup_widget(reboot_window, reboot, reboot_pos, reboot_color);
    setup_widget(refresh_window, refresh, refresh_pos, refresh_color);
    
    setup_button(shutdown, SHUTDOWN_IMG);
    setup_button(reboot, REBOOT_IMG);
    setup_button(refresh, REFRESH_IMG);
    
    g_signal_connect(G_OBJECT(shutdown), "clicked", G_CALLBACK(system_shutdown), NULL);
    g_signal_connect(G_OBJECT(reboot), "clicked", G_CALLBACK(system_reboot), NULL);
    g_signal_connect(G_OBJECT(refresh), "clicked", G_CALLBACK(setup_dialog), NULL);
    
    // Display the button panel
    gtk_widget_show(shutdown);
    gtk_widget_show(shutdown_window);
    gtk_widget_show(reboot);
    gtk_widget_show(reboot_window);
    gtk_widget_show(refresh);
    gtk_widget_show(refresh_window);
}
