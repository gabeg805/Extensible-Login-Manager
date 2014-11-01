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
//     Include the header file:
// 
//         #include "../hdr/Username.h"
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
//     setup_menu             - Setup user menu 
//     setup_label            - Setup user menu label
// 
//     usermenu_write_to_file - Write username to file
// 
//     get_username           - Get username/uid combination from the specified file
// 
//     set_username_entries   - Set entries in the user menu
// 
//     display_icon           - Display username icon 
//     display_username       - Display the username menu
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Display Username Icon
//     * Setup Username Menu
//     * Write User Menu Item to File
//     * Get Username-UID Combination
//     * Add Username Entries to the Menu
//     * Display Username Menu
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 10 2014 <> created
// 
//     gabeg Aug 14 2014 <> Updated gcc command in the header to include Utility.c
// 
//     gabeg Aug 20 2014 <> Moved the code inside Interface.c that displays the 
//                          username menu into the main Username.c module 
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries
// 
//     gabeg Oct 25 2014 <> Removed unnecessary structs to reduce memory usage. Also 
//                          removed the username setup function and used the 
//                          universal setup function instead. Added a setup function
//                          for the username menu.
// 
//     gabeg Oct 31 2014 <> Changed the way the username icon is displayed. Instead
//                          of forking and running pqiv, I added it to a GTK image
//                          widget and displayed the widget. 
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/glm.h"
#include "../hdr/Username.h"
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define XPOS           665
#define YPOS           197
#define IMG_XPOS       575
#define IMG_YPOS       190
#define WIDTH          0
#define HEIGHT         0
#define BG_WIN         (const GdkRGBA) {0, 0, 0, 0}
#define BG_MENU        (const GdkRGBA) {0, 0, 0, 0}
#define FG_WIN         (const GdkRGBA) {0, 0, 0, 0}
#define FG_MENU        (const GdkRGBA) {1, 1, 1, 1}
#define FSIZE          23*1024
#define FONT           "DejaVu Sans"
#define USERNAME_IMG   "/etc/X11/glm/img/interface/user.png"
#define USERNAME_LOG   "/etc/X11/glm/log/user.log"


// Declares
static void display_icon();
static void setup_menu(GtkWidget *widg, GtkWidget *label);
static void setup_label(GtkWidget *label);
static void usermenu_write_to_file(GtkMenu *item, GtkWidget *label);
static char ** get_username(char *file, int size);
static void set_username_entries(GtkWidget *menu, GtkWidget *label);
void display_username();



// ///////////////////////////////
// ///// SETUP USERNAME MENU /////
// ///////////////////////////////

// Setup dropdown menu that displays users
static void setup_menu(GtkWidget *widg, GtkWidget *label) {
    
    // Set username menu attributes
    GtkWidget *menu = gtk_menu_new();
    set_username_entries(menu, label);
    
    // Modify button style
    setup_label(label);
    
    // Attach the window manager menu to the dropdown menu
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(widg), menu);
    gtk_container_add(GTK_CONTAINER(widg), label);
    gtk_button_set_relief(GTK_BUTTON(widg), GTK_RELIEF_NONE);
}



// Setup the label for the dropdown menu
static void setup_label(GtkWidget *label) {
    
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

    // Set label text
    gtk_label_set_text(GTK_LABEL(label), USERNAME);
    gtk_label_set_attributes(GTK_LABEL(label), attrList);
}



// ////////////////////////////////////////
// ///// WRITE USER MENU ITEM TO FILE /////
// ////////////////////////////////////////

// Write to a file, which user to login as
static void usermenu_write_to_file(GtkMenu *item, GtkWidget *label) {
    const gchar *user = gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    
    snprintf(USERNAME, strlen(user)+1, "%s", (char*)user);
    file_write(USERNAME_LOG, "w", "%s\n", USERNAME);
    
    gtk_label_set_text(GTK_LABEL(label), USERNAME);
}



// ////////////////////////////////////////
// ///// GET USERNAME-UID COMBINATION /////
// ////////////////////////////////////////

// Get username/uid combination from the specified file
static char ** get_username(char *file, int size) {
    
    // Initialize username array
    char **array = malloc(sizeof(char*)*size);
    assert(array);
    array[0] = "0";
    
    // Open file for reading
    FILE *handle = fopen(file, "r");
    char line[150];
    
    // Loop through file lines
    int i = 1;
    while ( fgets(line, sizeof(line), handle) != NULL ) {
        
        // Information on file lines
        int uid;
        char *user = NULL;
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
                    assert(output);
                    snprintf(output, sz+1, user);
                    
                    array[i] = output;
                    ++i;
                }
            }
            
            count++;
        }
        
        free(orig);
        orig = NULL;
    }
    
    // Close file
    fclose(handle);
    
    // Put number of array elements into first index
    char val[5];
    snprintf(val, sizeof(val), "%d", (i-1));
    
    size_t sz = strlen(val);
    char *copy = malloc(sz+1);
    assert(copy);
    snprintf(copy, sz+1, val);
    
    array[0] = copy;
    
    return array;
}



// ////////////////////////////////////////////
// ///// ADD USERNAME ENTRIES TO THE MENU /////
// ////////////////////////////////////////////

// Set user name entries
static void set_username_entries(GtkWidget *menu, GtkWidget *label) {
    
    // Initialize user menu items
    GtkWidget *user;
    GSList *group = NULL;
    
    // Search for login users in passwd files
    int size = 20;
    char *files[] = {"/etc/shadow", "/etc/passwd", NULL};
    
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
        if ( strcmp(allusers[i], USERNAME) == 0 ) {
            user = gtk_radio_menu_item_new_with_label(NULL, allusers[i]);
            group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(user));
        } else if ( (q != 0) && (allusers[i] != NULL) )
            user = gtk_radio_menu_item_new_with_label(group, allusers[i]);
        else 
            p = 0;
        
        // Setup the menu items
        if ( p == 1 ) {
            free(allusers[i]);
            allusers[i] = NULL;
            
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
    allusers[0] = NULL;
    
    free(allusers);
    allusers = NULL;
}



// /////////////////////////////////
// ///// DISPLAY USERNAME MENU /////
// /////////////////////////////////

// Display username icon
void display_icon() {
    
    // Initialize username icon widget elements
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *img = gtk_image_new_from_file(USERNAME_IMG);
    
    // Define structs to hold widget information
    int pos[4] = {IMG_XPOS, IMG_YPOS, WIDTH, HEIGHT};
    const GdkRGBA color[4] = {BG_WIN, BG_MENU, FG_WIN, FG_MENU};
    
    // Setup icon widget
    setup_widget(win, img, pos, color);
    
    // Display icon widget
    gtk_widget_show(img);
    gtk_widget_show(win);
}



// Display the username menu
void display_username() {
    
    // Define username
    file_read(USERNAME_LOG, 1, 20, USERNAME);
    
    // Initialize username menu elements
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *widg = gtk_menu_button_new();
    GtkWidget *label = gtk_label_new("");
    
    // Define structs to hold widget information
    int pos[4] = {XPOS, YPOS, WIDTH, HEIGHT};
    const GdkRGBA color[4] = {BG_WIN, BG_MENU, FG_WIN, FG_MENU};
    
    // Display username icon
    display_icon();
    
    // Setup username menu widget
    setup_widget(win, widg, pos, color);
    setup_menu(widg, label);
    
    // Display the username menu
    gtk_widget_show(label);
    gtk_widget_show(widg);
    gtk_widget_show(win);
}
