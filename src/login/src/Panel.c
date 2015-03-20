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
//     setup_button    - Initialize refresh login button.
// 
//     system_shutdown - Command to shutdown computer.
//     system_reboot   - Command to reboot computer.
//     cancel_glm      - Command to close the refresh login popup.
//     refresh_glm     - Command to refresh the login manager.
//     quit_glm        - Command to quit the login manager.
// 
//     display_dialog  - Initialize refresh login button.
// 
//     display_item    - Display items in the button panel.
//     display_panel   - Display the panel.
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Setup Panel Buttons 
//     * System Commands
//     * Display Dialog
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
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file. Included the new functions that read in from
//                          the preferences file, "set_pref_pos" and 
//                          "set_pref_decor".
// 
//     gabeg Mar 19 2015 <> Utilized the universal setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Panel.h"

// Declares
static void setup_button(GtkWidget *widg, char *img);
static void system_shutdown();
static void system_reboot();
static void cancel_glm();
static void refresh_glm();
static void quit_glm();
static void display_dialog();
static void display_item();



// ///////////////////////////////
// ///// SETUP PANEL BUTTONS /////
// ///////////////////////////////

// Setup panel buttons and style
static void setup_button(GtkWidget *widg, char *img) {
    GtkWidget *icon = gtk_image_new_from_file(img);
    gtk_button_set_image(GTK_BUTTON(widg), icon);
    gtk_button_set_relief(GTK_BUTTON(widg), GTK_RELIEF_NONE);
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



// //////////////////////////
// ///// DISPLAY DIALOG /////
// //////////////////////////

// Display the GLM dialog
static void display_dialog() {
    
    // Log function start
    file_write(GLM_LOG, "a+", "%s: (%s:%d): Displaying GLM dialog...", 
               __FILE__, __FUNCTION__, __LINE__);
    
    // Declare the widget position, text, and decoration structs
    struct glmpos pos;
    
    // Define variables in preferences file
    set_pref_pos(PANEL_DIA_PREF,  &pos);
    
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
    gtk_window_move(GTK_WINDOW(window), pos.x, pos.y);
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
    g_signal_connect(G_OBJECT(refresh_button), "clicked", G_CALLBACK(refresh_glm),   NULL);
    g_signal_connect(G_OBJECT(quit_button),    "clicked", G_CALLBACK(quit_glm),      NULL);
    g_signal_connect(G_OBJECT(cancel_button),  "clicked", G_CALLBACK(cancel_glm),    window);
    g_signal_connect(window,                   "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Log function completion
    file_write(GLM_LOG, "a+", "Done\n");
}



// ////////////////////////////////
// ///// DISPLAY BUTTON PANEL /////
// ////////////////////////////////

// Display items in the button panel
static void display_item(char *file, void (*func)() ) {
    
    // Log function start
    file_write(GLM_LOG, "a+", "%s: (%s:%d): Displaying panel item...", 
               __FILE__, __FUNCTION__, __LINE__);
    
    // Allocate application attributes 
    struct glmapp app; 
    app.decor.img_file = malloc(READ_CHAR_LEN);
    
    // Define the application widget
    app.win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    app.widg = gtk_button_new();
    
    // Create the panel
    setup_widget(file, &app, "clicked", func);
    setup_button(app.widg, app.decor.img_file);
    
    // Log function completion
    file_write(GLM_LOG, "a+", "Done\n");
}


// Display the button panel 
void display_panel() {
    display_item(PANEL_SHUT_PREF, system_shutdown);
    display_item(PANEL_REB_PREF, system_reboot);
    display_item(PANEL_REF_PREF, display_dialog);
}
