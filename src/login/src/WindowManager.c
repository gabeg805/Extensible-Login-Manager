// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     WindowManager.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/WindowManager.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o WindowManager WindowManager.c Transparency.c FileRW.h `pkg-config gtk+-3.0 --cflags --libs`
//         $ ./WindowManager
// 
// 
// Purpose:
// 
//     Display a button for the user to choose between possible window managers for 
//     their login session.
// 
// 
// Keywords:
// 
//     N/A
// 
// 
// Functions:
// 
//     init_wm_root     - Initialize the root window
// 
//     wm_write_to_file - Write to a file, which window manager to use for the 
//                        session
// 
//     set_wm_entries   - Determine which window manager(s) the system has and add 
//                        them as entries to the menu
// 
// 
// File Structure:
// 
//     * Includes and Declares
//     * Initialize Window Manager Button
//     * Write Window Manager to File
//     * Add WM Entries to the Menu 
// 
// 
// Modification History:
// 	
//     gabeg Aug 02 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
// 
//     gabeg Aug 14 2014 <> Removed 'command_line' function and added it to FileRW.c
// 
// **********************************************************************************


// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/WindowManager.h"
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"
#include "../hdr/FileRW.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Declares
void init_wm_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu);
void wm_write_to_file(GtkMenu *item);
void set_wm_entries(GtkWidget *menu);



// ////////////////////////////////////////////
// ///// INITIALIZE WINDOW MANAGER BUTTON /////
// ////////////////////////////////////////////

// Initialize the root window and its objects
void init_wm_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(window), WINDOWMANAGER_XPOS, WINDOWMANAGER_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOWMANAGER_WIDTH*0, WINDOWMANAGER_HEIGHT*0);
    
    // Define and set color schemes
    const GdkRGBA bg_widget = {0, 0, 0, 0};
    const GdkRGBA fg_widget = {1, 1, 1, 1};
    set_color_and_opacity(window, dropmenu, bg_widget, fg_widget);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(WINDOWMANAGER_IMG);
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



// ////////////////////////////////////////
// ///// WRITE WINDOW MANAGER TO FILE ///// 
// ////////////////////////////////////////

// Write to a file, which window manager to use for the session
void wm_write_to_file(GtkMenu *item) {
    const gchar *sess = gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    file_write(WINDOWMANAGER_LOG, "w", "%s\n", (char *)sess);
}



// //////////////////////////////////////
// ///// ADD WM ENTRIES TO THE MENU /////
// //////////////////////////////////////

// Determine which window manager(s) the system has and add them as entries to the menu
void set_wm_entries(GtkWidget *menu) {
    
    // Initialize WM session items
    GtkWidget *session;
    GSList *group;
    
    // Get window manager information
    char **allwm  = command_line(WM_SES_CMD, 20);
    char *wmfocus = file_read(WINDOWMANAGER_LOG);
    int num = atoi(allwm[0]);
    
    // Define menu item counters
    int i = 1, q = 0, p = 0;    
    while (1) {
        
        // Compare label with the focus label
        p = 1;
        if ( strcmp(allwm[i], wmfocus) == 0 ) {
            session = gtk_radio_menu_item_new_with_label(NULL, allwm[i]);
            group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(session));
        } else if ( (q != 0) && (strcmp(allwm[i], "") != 0) ) 
            session = gtk_radio_menu_item_new_with_label(group, allwm[i]);
        else 
            p = 0;
        
        // Setup the menu items
        if ( p == 1 ) {
            free(allwm[i]);
            
            gtk_menu_attach(GTK_MENU(menu), session, 0, 1, q, q+1);
            gtk_widget_show(session);
            g_signal_connect(G_OBJECT(session), "activate", G_CALLBACK(wm_write_to_file), NULL);
            
            if ( (++q) == num ) 
                break;
        }
        
        // Increment counter
        if ( (++i) > num )
            i = 1;
    }
        
    // Freeing up the memory
    free(allwm[0]);
    free(allwm);
    free(wmfocus);
}
