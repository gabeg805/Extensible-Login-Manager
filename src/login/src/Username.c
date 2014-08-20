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
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"
#include "../hdr/FileRW.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Declares
void init_usermenu_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu, GtkWidget *label);
void init_userlabel(GtkWidget *label);
void usermenu_write_to_file(GtkMenu *item, GtkWidget *label);
char ** get_username(char *file, int size);
void set_username_entries(GtkWidget *menu, GtkWidget *label);



// ////////////////////////////////////
// ///// INITIALIZE USERNAME MENU /////
// ////////////////////////////////////

// Initialize the root window and its objects
void init_usermenu_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu, GtkWidget *label) {
    
    // Set username icon
    if ( !fork() )
        execl(PQIV, PQIV, "-c", "-i", "-P", "575,190", USERNAME_IMG, NULL);
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(window), USERNAME_XPOS, USERNAME_YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), USERNAME_WIDTH*0, USERNAME_HEIGHT*0);
    
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
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(USERNAME_FONT);
    PangoAttribute *attrSize = pango_attr_size_new(USERNAME_FSIZE);
    PangoAttribute *attrColor = pango_attr_foreground_new(65535, 65535, 65535);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrColor);

    // Set label text
    char *user = file_read(USERNAME_LOG);
    gtk_label_set_text(GTK_LABEL(label), user);
    gtk_label_set_attributes(GTK_LABEL(label), attrList);
    
    free(user);
}



// ////////////////////////////////////////
// ///// WRITE USER MENU ITEM TO FILE ///// 
// ////////////////////////////////////////

// Write to a file, which user to login as
void usermenu_write_to_file(GtkMenu *item, GtkWidget *label) {
    const gchar *user = gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    file_write(USERNAME_LOG, "w", "%s\n", (char *)user);
    gtk_label_set_text(GTK_LABEL(label), user);
}



// ////////////////////////////////////////
// ///// GET USERNAME-UID COMBINATION /////
// ////////////////////////////////////////

// Get username/uid combination from the specified file
char ** get_username(char *file, int size) {
    
    // Initialize username array
    char **array = (char**)malloc(sizeof(char*)*size);
    array[0] = "0";
    
    // Open file for reading
    FILE *handle = fopen(file, "r");
    char line[150];
    
    // Loop through file lines
    int i = 1;
    while ( fgets(line, sizeof(line), handle) != NULL ) {
        
        // Information on file lines
        int uid;
        char *user;
        char *sep;
        char *buffer, *orig;
        buffer = orig = strdup(line);
        
        
        // Loop through words (delimited by ':') on line
        int count = 0;
        while ( (count < 3) && (sep = strsep(&buffer, ":")) ) {
            if ( count == 0 ) 
                user = sep;
            else if ( count == 1 ) 
                ;
            else if ( count == 2 ) {
                uid = atoi(sep);
                
                if ( (strcmp(user, "root") == 0) && (uid != 0) )
                    return array;
                
                if ( (uid == 0) || ((uid >= 1000) && (uid <= 9999)) ) {
                    size_t sz = strlen(user);
                    char *output = malloc(sz+1);
                    snprintf(output, sz+1, user);
                    
                    array[i] = output;
                    ++i;
                }
            }
            
            count++;
        }
        
        free(orig);
    }
    
    // Close file
    fclose(handle);
    
    // Put number of array elements into first index
    char val[5];
    snprintf(val, sizeof(val), "%d", (i-1));
    
    size_t sz = strlen(val);
    char *copy = malloc(sz+1);
    snprintf(copy, sz+1, val);
    
    array[0] = copy;
    
    return array;
}



// ////////////////////////////////////////////
// ///// ADD USERNAME ENTRIES TO THE MENU /////
// ////////////////////////////////////////////

// Set user name entries
void set_username_entries(GtkWidget *menu, GtkWidget *label) {
    
    // Initialize user menu items
    GtkWidget *user;
    GSList *group;
    
    // Search for login users in passwd files
    int size = 20;
    char *files[] = {"/etc/shadow", "/etc/passwd", NULL};
    char *userfocus = file_read(USERNAME_LOG);
    
    // Set usernames from file
    int num;
    char **allusers;
    allusers = get_username(files[0], size);
    num = atoi(allusers[0]);
    
    // Wrong file was used above, use the other file
    if ( num == 0 ) {
        allusers = get_username(files[1], size);
        num = atoi(allusers[0]);
    }
    
    // Define menu item counters
    int i = 1, q = 0, p = 0;
    while (1) {
        
        // Compare label with the focus label
        p = 1; 
        if ( strcmp(allusers[i], userfocus) == 0 ) {
            user = gtk_radio_menu_item_new_with_label(NULL, allusers[i]);
            group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(user));
        } else if ( (q != 0) && (strcmp(allusers[i], "") != 0) )
            user = gtk_radio_menu_item_new_with_label(group, allusers[i]);
        else 
            p = 0;
        
        // Setup the menu items
        if ( p == 1 ) {
            free(allusers[i]);
            
            // Attach window manager entries to the menu
            gtk_menu_attach(GTK_MENU(menu), user, 0, 1, q, q+1);
            gtk_widget_show(user);
            
            // Add menu entry signal
            g_signal_connect(G_OBJECT(user), "activate", G_CALLBACK(usermenu_write_to_file), label);
            
            if ( (++q) == num ) 
                break;
        }
        
        // Increment counter
        if ( (++i) > num )
            i = 1;
    }
    
    // Free the memory
    free(allusers[0]);
    free(allusers);
    free(userfocus);
}
