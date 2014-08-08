// 
// Created By: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// Name:
// 
//     WindowManager.c
// 
// 
// Syntax: 
// 
//     ./WindowManager
// 
// 
// Purpose:
// 
//     Displays a button for the user to choose between possible window managers for 
//     their login session.
// 
// 
// Keywords:
// 
//     Unknown
// 
// 
// Functions:
// 
//     init_wm_root        - Initialize the root window
//     set_wm_color        - Set the color scheme for the root window
//     wm_write_to_file    - Write to a file, which window manager to use for the 
//                           session
//     set_wm_entries      - Determine which window manager(s) the system has and add 
//                           them as entries to the menu
//     main                - Display the window manager chooser
// 
// 
// File Structure:
// 
//     * Includes and Declares
//     * Initialization
//     * WM Command
//     * Add WM Entries to the Menu 
//     * Display the WM Chooser
// 
// 
// Modification History:
// 	
//     gabeg Aug 02 2014 <> created
// 
// **********************************************************************************


// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/WindowManager.h"
#include "../hdr/CommandLine.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#define   XPOS       770
#define   YPOS       310
#define   WIDTH      30
#define   HEIGHT     30
#define   IMG_FILE   "/etc/X11/glm/img/settings.png"
#define   SES_FILE   "/etc/X11/glm/log/session.log"


// Declares
void init_wm_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu);
void set_wm_color(GtkWidget *window, GtkWidget *dropmenu);
void wm_write_to_file(GtkMenu *item);
void set_wm_entries(GtkWidget *menu);



// //////////////////////////
// ///// INITIALIZATION /////
// //////////////////////////

// Initialize the root window and its objects
void init_wm_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu) {
    
    // Set window attributes
    gtk_window_set_title(GTK_WINDOW(window), "Window Manager");
    gtk_window_move(GTK_WINDOW(window), XPOS, YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
    
    // Define and set color schemes
    set_wm_color(window, dropmenu);
    
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(IMG_FILE);
    gtk_button_set_image(GTK_BUTTON(dropmenu), image);
    gtk_button_set_relief(GTK_BUTTON(dropmenu), GTK_RELIEF_NONE);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    
    
    // Attach the window manager menu to the dropdown menu
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(dropmenu), menu);
    gtk_container_add(GTK_CONTAINER(window), dropmenu);
    
    
    // Attempt to enable window transparency
    enable_transparency(window);
    
    // GTK signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// ////////////////////////////////////////////
// ///// SET COLOR SCHEME FOR ROOT WINDOW /////
// ////////////////////////////////////////////

// Set the color scheme for the root window
void set_wm_color(GtkWidget *window, GtkWidget *dropmenu) {
    
    // Define color scheme
    const GdkRGBA bg_window = {1, 1, 1, 0};
    const GdkRGBA fg_window = {1, 1, 1, 0};
    const GdkRGBA bg_menu = {1, 1, 1, 0};
    const GdkRGBA fg_menu = {1, 1, 1, 1};
    
    // Set color scheme
    gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &bg_window);
    gtk_widget_override_background_color(dropmenu, GTK_STATE_FLAG_NORMAL, &bg_menu);
    gtk_widget_override_color(window, GTK_STATE_FLAG_NORMAL, &fg_window);
    gtk_widget_override_color(dropmenu, GTK_STATE_FLAG_NORMAL, &fg_menu);
}



// //////////////////////
// ///// WM COMMAND ///// 
// //////////////////////

// Write to a file, which window manager to use for the session
void wm_write_to_file(GtkMenu *item) {
    
    // Chosen session to start
    const gchar *sess = gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    
    // Command to write to file
    char cmd[50];
    snprintf(cmd, sizeof(cmd), "%s %s %s %s", "echo", sess, ">", SES_FILE);    
    
    // Write chosen session to file
    system(cmd);
}



// //////////////////////////////////////
// ///// ADD WM ENTRIES TO THE MENU /////
// //////////////////////////////////////

// Determine which window manager(s) the system has and add them as entries to the menu
void set_wm_entries(GtkWidget *menu) {
    
    // Initialize WM session menu item
    GtkWidget *session;
    
    // Get window manager string
    char *allwm = command_line("ls -1 /usr/share/xsessions/ | sed 's/.desktop//'");
    char wm[strlen(allwm)];
    GSList *group;
    
    // Determine window manager entries and add them as entries to the menu
    int i=0, j=0, count=0;    
    for (i = 0; i < strlen(allwm); i++) {
        
        // Window manager string array delimeter, space separates one string from the next
        if ( allwm[i] == ' ' ) {
            
            // Create the menu items and add them to the same group
            if (count == 0) {
                session = gtk_radio_menu_item_new_with_label(NULL, wm);
                group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(session));
            } else
                session = gtk_radio_menu_item_new_with_label(group, wm);
            
            // Attach window manager entries to the menu
            gtk_menu_attach(GTK_MENU(menu), session, 0, 1, count, count+1);
            gtk_widget_show(session);
            
            // Add menu entry signal
            g_signal_connect(G_OBJECT(session), "activate", G_CALLBACK(wm_write_to_file), NULL);
            
            j = 0;
            count++;
        } else {
            wm[j] = allwm[i];
            wm[j+1] = '\0';
            j++;
        }
    }
    
    // Freeing up the memory
    free(allwm);
}



// //////////////////////////////////
// ///// DISPLAY THE WM CHOOSER /////
// //////////////////////////////////

// Display the window manager chooser
/* int main(int argc, char *argv[]) { */

/*     // Initialize GTK toolkit */
/*     gtk_init(&argc, &argv); */
    
/*     // Define window and drawing area */
/*     GtkWidget *wm_window = gtk_window_new(GTK_WINDOW_TOPLEVEL), */
/*         *wm_dropmenu     = gtk_menu_button_new(), */
/*         *wm_menu         = gtk_menu_new(), */
/*         *session; */
    
/*     // Initialize the root window and its objects */
/*     init_wm_root(wm_window, wm_dropmenu, wm_menu); */
    
/*     // Attach the window manager menu to the dropdown menu */
/*     get_wm_entries(session, wm_menu); */
    
/*     // Display GTK window */
/*     gtk_widget_show(wm_dropmenu); */
/*     gtk_widget_show(wm_window); */
/*     gtk_main(); */
    
/*     return 0; */
/* } */
