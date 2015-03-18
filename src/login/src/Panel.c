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
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file. Included the new functions that read in from
//                          the preferences file, "set_pref_pos" and 
//                          "set_pref_decor".
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Panel.h"

// Declares
static void setup_button(GtkWidget *widg, char *pic);
static void setup_dialog();
static void system_shutdown();
static void system_reboot();
static void cancel_glm();
static void refresh_glm();
static void quit_glm();

static struct glmpos SHUT_POS;
static struct glmpos REB_POS;
static struct glmpos REF_POS;
static struct glmpos DIA_POS;

static struct glmdecor SHUT_DECOR;
static struct glmdecor REB_DECOR;
static struct glmdecor REF_DECOR;



// ///////////////////////////////
// ///// SETUP PANEL BUTTONS /////
// ///////////////////////////////

// Setup panel buttons and style
static void setup_button(GtkWidget *widg, char *pic) {
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
    gtk_window_move(GTK_WINDOW(window), DIA_POS.x, DIA_POS.y);
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
    
    // Allocate memory for strings
    SHUT_DECOR.img_file = malloc(READ_CHAR_LEN);
    REB_DECOR.img_file  = malloc(READ_CHAR_LEN);
    REF_DECOR.img_file  = malloc(READ_CHAR_LEN);
    
    // Define variables in preferences file
    set_pref_pos(PANEL_SHUT_PREF, &SHUT_POS);
    set_pref_pos(PANEL_REB_PREF,  &REB_POS);
    set_pref_pos(PANEL_REF_PREF,  &REF_POS);
    set_pref_pos(PANEL_DIA_PREF,  &DIA_POS);
    
    set_pref_decor(PANEL_SHUT_PREF, &SHUT_DECOR);
    set_pref_decor(PANEL_REB_PREF,  &REB_DECOR);
    set_pref_decor(PANEL_REF_PREF,  &REF_DECOR);
    
    // #)@(#*$@#)($* CAN MAKE THIS INTO MULTIPLE FUNCTIONS @)#(*$@#)
    
    // Initialize button panel items
    GtkWidget *shut_win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *reb_win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *ref_win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *shut = gtk_button_new();
    GtkWidget *reb  = gtk_button_new();
    GtkWidget *ref  = gtk_button_new();
    
    
    // Setup widgets
    set_widget_color(shut_win, shut, SHUT_DECOR);
    set_widget_color(reb_win,  reb,  REB_DECOR);
    set_widget_color(ref_win,  ref,  REF_DECOR);
    
    setup_widget(shut_win, shut, SHUT_POS);
    setup_widget(reb_win,  reb,  REB_POS);
    setup_widget(ref_win,  ref,  REF_POS);
    
    setup_button(shut, SHUT_DECOR.img_file);
    setup_button(reb,  REB_DECOR.img_file);
    setup_button(ref,  REF_DECOR.img_file);
    
    g_signal_connect(G_OBJECT(shut), "clicked", G_CALLBACK(system_shutdown), NULL);
    g_signal_connect(G_OBJECT(reb),   "clicked", G_CALLBACK(system_reboot),   NULL);
    g_signal_connect(G_OBJECT(ref),  "clicked", G_CALLBACK(setup_dialog),    NULL);
    
    
    // Display the button panel
    gtk_widget_show(shut);
    gtk_widget_show(reb);
    gtk_widget_show(ref);
    gtk_widget_show(shut_win);
    gtk_widget_show(reb_win);
    gtk_widget_show(ref_win);
}
