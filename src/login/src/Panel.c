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
#define BG_WINDOW         (const GdkRGBA) {0, 0, 0, 0}
#define FG_WINDOW         (const GdkRGBA) {0, 0, 0, 0}
#define FG_SHUTDOWN       (const GdkRGBA) {1, 1, 1, 1}
#define FG_REBOOT         (const GdkRGBA) {1, 1, 1, 1}
#define FG_REFRESH        (const GdkRGBA) {1, 1, 1, 1}
#define SHUTDOWN_IMG      "/etc/X11/glm/img/interface/shutdown.png"
#define REBOOT_IMG        "/etc/X11/glm/img/interface/reboot.png"
#define REFRESH_IMG       "/etc/X11/glm/img/interface/refresh.png"


// Declares
static void setup_button(struct glmstruct *gui, void (*func)(void), char *pic);
static void setup_dialog();
static void system_shutdown();
static void system_reboot();
static void cancel_glm();
static void refresh_glm();
static void quit_glm();
void display_panel();



// ////////////////////////////////////
// ///// INITIALIZE PANEL BUTTONS /////
// ////////////////////////////////////

// Setup the buttons on the  panel
static void setup_button(struct glmstruct *gui, void (*func)(void), char *pic) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(gui->win), gui->pos->x, gui->pos->y);
    gtk_window_set_default_size(GTK_WINDOW(gui->win), gui->pos->width, gui->pos->height);
    
    // Set power button attributes
    set_stuff(gui);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(pic);
    gtk_button_set_image(GTK_BUTTON(gui->widg), image);
    gtk_button_set_relief(GTK_BUTTON(gui->widg), GTK_RELIEF_NONE);
    
    // Add the clock to the root window 
    gtk_container_add(GTK_CONTAINER(gui->win), gui->widg);
        
    // Enable transparency
    enable_transparency(gui->win);
    
    // GTK signal
    g_signal_connect(G_OBJECT(gui->widg), "clicked", G_CALLBACK(func), NULL);
    g_signal_connect(gui->win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
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
    gtk_widget_destroy(window);
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
    
    // Setup structs to hold widget information
    struct glmcolor *shutdown_color = setup_color_struct(BG_WINDOW, FG_WINDOW, BG_SHUTDOWN, FG_SHUTDOWN);
    struct glmcolor *reboot_color   = setup_color_struct(BG_WINDOW, FG_WINDOW, BG_REBOOT,   FG_REBOOT);
    struct glmcolor *refresh_color  = setup_color_struct(BG_WINDOW, FG_WINDOW, BG_REFRESH,  FG_REFRESH);
    struct glmpos *shutdown_pos = setup_pos_struct(SHUTDOWN_XPOS, SHUTDOWN_YPOS, SHUTDOWN_WIDTH, SHUTDOWN_HEIGHT);
    struct glmpos *reboot_pos   = setup_pos_struct(REBOOT_XPOS, REBOOT_YPOS, REBOOT_WIDTH, REBOOT_HEIGHT);
    struct glmpos *refresh_pos  = setup_pos_struct(REFRESH_XPOS, REFRESH_YPOS, REFRESH_WIDTH, REFRESH_HEIGHT);    
    struct glmstruct *shutdown_gui = setup_struct(shutdown_window, shutdown, shutdown_pos, shutdown_color, NULL);
    struct glmstruct *reboot_gui   = setup_struct(reboot_window,   reboot,   reboot_pos, reboot_color, NULL);
    struct glmstruct *refresh_gui  = setup_struct(refresh_window,  refresh,  refresh_pos, refresh_color, NULL);
    
    // Setup buttons on the panel
    setup_button(shutdown_gui, system_shutdown, SHUTDOWN_IMG);
    setup_button(reboot_gui, system_reboot, REBOOT_IMG);
    setup_button(refresh_gui, setup_dialog, REFRESH_IMG);
    
    // Display the button panel
    gtk_widget_show(shutdown_gui->widg);
    gtk_widget_show(shutdown_gui->win);
    gtk_widget_show(reboot_gui->widg);
    gtk_widget_show(reboot_gui->win);
    gtk_widget_show(refresh_gui->widg);
    gtk_widget_show(refresh_gui->win);
}
