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
//     Include the header file:
// 
//         #include "../hdr/WindowManager.h"
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
//     setup_menu             - Setup window manager dropdown menu
// 
//     wm_write_to_file       - Write to a file, which window manager to use for the 
//                              session
// 
//     set_wm_entries         - Determine which window manager(s) the system has and add 
//                              them as entries to the menu
// 
//     display_window_manager - Display the window manager button
// 
// 
// File Structure:
// 
//     * Includes and Declares
//     * Setup Window Manager Button
//     * Write Window Manager to File
//     * Add WM Entries to the Menu 
//     * Display Window Manager
// 
// 
// Modification History:
// 	
//     gabeg Aug 02 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header.
// 
//     gabeg Aug 14 2014 <> Removed 'command_line' function and added it to Utility.c
// 
//     gabeg Aug 20 2014 <> Moved the code inside Interface.c that displays the 
//                          window manager button into the main WindowManager.c 
//                          module.
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries.
// 
//     gabeg Oct 25 2014 <> Removed window manager setup function and used the 
//                          universal setup function instead. Included the #define 
//                          statements so that I don't need a general config file.
// 
//     gabeg Oct 31 2014 <> Modified "set_wm_entries" so that the modified 
//                          "command_line" function would work.
// 
//     gabeg Nov 01 2014 <> Changed "command_line" return value to be (char *) so
//                          altered code to reflect that change.
// 
// **********************************************************************************


// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"
#include "../hdr/WindowManager.h"
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define XPOS          770
#define YPOS          315
#define WIDTH         30
#define HEIGHT        30
#define BG_WIN        (const GdkRGBA) {0, 0, 0, 0}
#define FG_WIN        (const GdkRGBA) {0, 0, 0, 0}
#define BG_WM         (const GdkRGBA) {0, 0, 0, 0}
#define FG_WM         (const GdkRGBA) {1, 1, 1, 1}
#define WM_IMG        "/etc/X11/glm/img/interface/settings.png"
#define WM_SES_CMD    "ls -1 /usr/share/xsessions/ | sed 's/.desktop//'"
#define SESSION_LOG   "/etc/X11/glm/log/session.log"


// Declares
static void setup_menu(GtkWidget *widg, GtkWidget *menu);
static void wm_write_to_file(GtkMenu *item);
static void set_wm_entries(GtkWidget *menu);
void display_window_manager();



// /////////////////////////////////////
// ///// SETUP WINDOW MANAGER MENU /////
// /////////////////////////////////////

// Setup window manager dropdown menu button
static void setup_menu(GtkWidget *widg, GtkWidget *menu) {
    
    // Set entries in window manager dropdown menu
    set_wm_entries(menu);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(WM_IMG);
    gtk_button_set_image(GTK_BUTTON(widg), image);
    gtk_button_set_relief(GTK_BUTTON(widg), GTK_RELIEF_NONE);
    
    // Make menu popup
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(widg), menu);
}



// ////////////////////////////////////////
// ///// WRITE WINDOW MANAGER TO FILE ///// 
// ////////////////////////////////////////

// Write to a file, which window manager to use for the session
static void wm_write_to_file(GtkMenu *item) {
    const gchar *sess = gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    snprintf(SESSION, strlen(sess)+1, "%s", (char*)sess);
    file_write(SESSION_LOG, "w", "%s\n", SESSION);
}



// //////////////////////////////////////
// ///// ADD WM ENTRIES TO THE MENU /////
// //////////////////////////////////////

// Determine which window manager(s) the system has and add them as entries to the menu
static void set_wm_entries(GtkWidget *menu) {
    
    // Initialize WM session items
    GtkWidget *sesh;
    GSList *group = NULL;
    
    // Get all window managers in one string 
    char *wmstr = command_line(WM_SES_CMD, 20, 100);
    
    // Break up window manager string into array of strings (based on newline char)
    int k = 0;
    int count = count_char(wmstr, '\n') + 1;
    char *allwm[count];
    char *sep;
    char *wm = strdup(wmstr);
    
    while ( (sep=strsep(&wm, "\n")) != NULL ) {
        allwm[k] = sep;
        ++k;
    }
    
    // Create menu labels based on the window managers that were found
    int i = 0, q = 0, p = 0;
    while (1) {
        
        // Compare label with the focus label
        p = 1;
        if ( strcmp(allwm[i], SESSION) == 0 ) {
            sesh = gtk_radio_menu_item_new_with_label(NULL, allwm[i]);
            group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(sesh));
        } else if ( (q != 0) && (allwm[i] != NULL) )
            sesh = gtk_radio_menu_item_new_with_label(group, allwm[i]);
        else
            p = 0;
        
        // Setup the menu items
        if ( p == 1 ) {
            allwm[i] = NULL;
            
            gtk_menu_attach(GTK_MENU(menu), sesh, 0, 1, q, q+1);
            gtk_widget_show(sesh);
            g_signal_connect(G_OBJECT(sesh), "activate", G_CALLBACK(wm_write_to_file), NULL);
                    
            if ( (++q) == count )
                break;
        }
        
        // Increment counter
        if ( (++i) >= count )
            i = 0;
    }
    
    // Free used memory
    free(wm);
    free(wmstr);
}



// //////////////////////////////////
// ///// DISPLAY WINDOW MANAGER /////
// //////////////////////////////////

// Display the window manager button
void display_window_manager() {
    
    // Define session
    SESSION = file_read(SESSION_LOG, 1, 20);
    
    // Initialize window manager elements
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *widg = gtk_menu_button_new();
    GtkWidget *menu = gtk_menu_new();
    
    // Define structs to hold widget information
    int pos[4] = {XPOS, YPOS, WIDTH, HEIGHT};
    const GdkRGBA color[4] = {BG_WIN, BG_WM, FG_WIN, FG_WM};
    
    // Setup window manager button
    setup_widget(win, widg, pos, color);
    setup_menu(widg, menu);
    
    // Display the window manager button
    gtk_widget_show(widg);
    gtk_widget_show(win);
}
