// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Username.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/Username.h"
// 
//     With a 'main' function, execute the following:
// 
//         $ gcc -o Username Username.c Transparency.c FileRW.c `pkg-config gtk+-3.0 --cflags --libs`
//         $ ./Username
// 
// 
// PURPOSE:
// 
//     Display dropdown menu that has all users that are able to login to the system.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     init_usermenu_root     - Initialize user menu root window
//     init_userlabel         - Initialize user menu label
// 
//     usermenu_write_to_file - Write username to file
// 
//     set_username_entries   - Set entries in the user menu
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Username Menu
//     * Write User Menu Item to File
//     * Add Username Entries to the Menu
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 10 2014 <> created
// 
//     gabeg Aug 14 2014 <> Updated gcc command in the header to include FileRW.c
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Username.h"
#include "../hdr/Transparency.h"
#include "../hdr/FileRW.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define   XPOS        665
#define   YPOS        197
#define   WIDTH       150
#define   HEIGHT      50
#define   FONT        "DejaVu Sans"
#define   FSIZE       23*1024
#define   USER_FILE   "/etc/X11/glm/log/user.log"
#define   IMG_FILE    "/etc/X11/glm/img/interface/user.png"
#define   IMG_COM     "/usr/bin/pqiv"


// Declares
void init_usermenu_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu, GtkWidget *label);
void init_userlabel(GtkWidget *label);
void usermenu_write_to_file(GtkMenu *item, GtkWidget *label);
void set_username_entries(GtkWidget *menu, GtkWidget *label);



// ////////////////////////////////////
// ///// INITIALIZE USERNAME MENU /////
// ////////////////////////////////////

// Initialize the root window and its objects
void init_usermenu_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu, GtkWidget *label) {
    
    // Set username icon
    if ( !fork() ) 
        execl(IMG_COM, IMG_COM, "-c", "-i", "-P", "575,190", IMG_FILE, NULL);
    
    // Set window attributes
    gtk_window_set_title(GTK_WINDOW(window), "Username");
    gtk_window_move(GTK_WINDOW(window), XPOS, YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH*0, HEIGHT*0);
    
    // Define and set color schemes
    const GdkRGBA bg_widget = {0, 0, 0, 0};
    const GdkRGBA fg_widget = {1, 1, 1, 1};
    set_color_and_opacity(window, dropmenu, bg_widget, fg_widget);
    
    // Modify button style
    init_userlabel(label);
    gtk_button_set_relief(GTK_BUTTON(dropmenu), GTK_RELIEF_NONE);
    
    // Attach the window manager menu to the dropdown menu
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(dropmenu), menu);
    gtk_container_add(GTK_CONTAINER(dropmenu), label);
    gtk_container_add(GTK_CONTAINER(window), dropmenu);
    
    // attempt to enable window transparency
    enable_transparency(window);
    
    // GTK signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// Initialze the label for the user menu
void init_userlabel(GtkWidget *label) {
    
    // Set label text
    gtk_label_set_text(GTK_LABEL(label), "User");
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(FONT);
    PangoAttribute *attrSize = pango_attr_size_new(FSIZE);
    PangoAttribute *attrColor = pango_attr_foreground_new(65535, 65535, 65535);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrColor);

    gtk_label_set_attributes(GTK_LABEL(label), attrList);
}



// ////////////////////////////////////////
// ///// WRITE USER MENU ITEM TO FILE ///// 
// ////////////////////////////////////////

// Write to a file, which user to login as
void usermenu_write_to_file(GtkMenu *item, GtkWidget *label) {
    const gchar *user = gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    file_write(USER_FILE, (char *)user, "%s\n");
    
    gtk_label_set_text(GTK_LABEL(label), user);
}



// ////////////////////////////////////////////
// ///// ADD USERNAME ENTRIES TO THE MENU /////
// ////////////////////////////////////////////

// Set user name entries
void set_username_entries(GtkWidget *menu, GtkWidget *label) {
    
    // Initialize WM session menu item
    GtkWidget *session;
    GSList *group;
    
    // Possible files containing list of regular users
    char *files[] = {"/etc/shadow", "/etc/passwd", NULL};
    int c = 0;
    
    // Loop through files
    int i = 0;
    while ( files[i] != NULL ) {
        
        // Open file for reading
        FILE *handle = fopen(files[i], "r");
        char line[1024];
        
        // Loop through file lines
        int trigger = 0;
        while ( fgets(line, sizeof(line), handle) != NULL ) {
            
            // Information on file lines
            char *buffer = strdup(line);
            char *sep;
            char *user;
            int uid;
            
            // Loop through words (delimited by ':') on line
            int count = 0;
            int flag = 0;
            while ( (count < 3) && (sep = strsep(&buffer, ":")) ) {
                
                // Value dependent actions
                switch (count) {
                    
                    // Define user and look for 'root' on line 
                case 0:
                    user = sep;
                    if ( strcmp(user, "root") == 0 ) 
                        flag = 1;
                    else
                        flag = 0;
                    break;
                    
                    // Check for 'x' in second word delimited by ':'
                case 1:
                    if ( (flag == 1) && (strcmp(sep, "x") == 0) ) 
                        flag = 1;
                    else
                        flag = 0;
                    break;
                    
                    // If 'x' was there, then this is the correct file, print out user
                case 2:
                    if ( trigger == 1 ) {
                        uid = atoi(sep);
                        
                        // Create the menu items and add them to the same group
                        if ( (uid >= 1000) && (uid <= 9999) ) {
                            
                            if (c == 0) {
                                // First user
                                session = gtk_radio_menu_item_new_with_label(NULL, user);
                                group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(session));
                            } else
                                // Othere users
                                session = gtk_radio_menu_item_new_with_label(group, user);
                            
                            // Attach window manager entries to the menu
                            gtk_menu_attach(GTK_MENU(menu), session, 0, 1, c, c+1);
                            gtk_widget_show(session);
                            
                            // Add menu entry signal
                            g_signal_connect(G_OBJECT(session), "activate", G_CALLBACK(usermenu_write_to_file), label);
                            
                            c++;
                        }
                    }
                    break;
                }
                
                
                // Set trigger which specifies whether this is the correct file or not
                if ( (count == 1) && (flag == 1) && (trigger == 0) ) 
                    trigger = 1;
                else if ( (count == 1) && (flag == 0) && (trigger == 0) ) {
                    trigger = 0;
                    break;
                }
                
                // Increment count
                count++;
            }
            
            // File was bad, break out of loop to switch to next file
            if ( trigger == 0 )
                break;
        }
        
        // Close file and increment file index
        fclose(handle);
        i++;
    }
        
}
