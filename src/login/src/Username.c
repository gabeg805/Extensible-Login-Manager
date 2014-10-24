// * change line 251
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
//     init_usermenu_root     - Initialize user menu root window
//     init_userlabel         - Initialize user menu label
// 
//     usermenu_write_to_file - Write username to file
// 
//     set_username_entries   - Set entries in the user menu
// 
//     display_username       - Display the username menu
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Initialize Username Menu
//     * Write User Menu Item to File
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
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Username.h"
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"
#include "../hdr/Utility.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define XPOS        665
#define YPOS        197
#define WIDTH       0
#define HEIGHT      0
#define BG_MENU     (const GdkRGBA) {0, 0, 0, 0}
#define FG_MENU     (const GdkRGBA) {1, 1, 1, 1}
#define BG_WINDOW   (const GdkRGBA) {0, 0, 0, 0}
#define FG_WINDOW   (const GdkRGBA) {0, 0, 0, 0}
#define FSIZE       23*1024
#define FONT        "DejaVu Sans"


// Declares
static void init_usermenu_root(struct glmgui *gui, struct glmpos *pos, struct glmcolor *color);
static void init_userlabel(GtkWidget *label);
static void usermenu_write_to_file(GtkMenu *item, GtkWidget *label);
static char ** get_username(char *file, int size);
static void set_username_entries(GtkWidget *menu, GtkWidget *label);
void display_username();



// ////////////////////////////////////
// ///// INITIALIZE USERNAME MENU /////
// ////////////////////////////////////

// Initialize the root window and its objects
void init_usermenu_root(struct glmgui *gui, struct glmpos *pos, struct glmcolor *color) {
    
    // Set username icon
    if ( !fork() )
        execl(PQIV, PQIV, "-c", "-i", "-P", "575,190", gui->img, NULL);
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(gui->win), pos->x, pos->y);
    gtk_window_set_default_size(GTK_WINDOW(gui->win), pos->width, pos->height);
    
    // Set username menu attributes
    GtkWidget *menu = gtk_menu_new();
    set_color_and_opacity(gui->win, gui->widg, color->bgwidg, color->fgwidg);
    set_username_entries(menu, gui->extra);
    
    // Modify button style
    init_userlabel(gui->extra);
    
    // Attach the window manager menu to the dropdown menu
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(gui->widg), menu);
    gtk_container_add(GTK_CONTAINER(gui->widg), gui->extra);
    gtk_container_add(GTK_CONTAINER(gui->win), gui->widg);
    gtk_button_set_relief(GTK_BUTTON(gui->widg), GTK_RELIEF_NONE);
    
    // attempt to enable window transparency
    enable_transparency(gui->win);
    
    // GTK signals
    g_signal_connect(gui->win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}



// Initialze the label for the user menu
void init_userlabel(GtkWidget *label) {
    
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
void usermenu_write_to_file(GtkMenu *item, GtkWidget *label) {
    const gchar *user = gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    USERNAME = (char*)user;
    file_write(USERNAME_LOG, "w", "%s\n", USERNAME);
    gtk_label_set_text(GTK_LABEL(label), USERNAME);
}



// ////////////////////////////////////////
// ///// GET USERNAME-UID COMBINATION /////
// ////////////////////////////////////////

// Get username/uid combination from the specified file
char ** get_username(char *file, int size) {
    
    // Initialize username array
    char **array = malloc(sizeof(char*)*size);
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
        orig = NULL;
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
    char *userfocus = strdup(USERNAME);
    
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
    
    free(userfocus);
    userfocus = NULL; 
}



// /////////////////////////////////
// ///// DISPLAY USERNAME MENU /////
// /////////////////////////////////

// Display the username menu
void display_username() {
    
    // Initialize username menu elements
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *dropmenu = gtk_menu_button_new();
    GtkWidget *label = gtk_label_new("");
    
    // Setup username menu
    struct glmgui *gui = setup_gui_struct(window, dropmenu, label, USERNAME_IMG);
    struct glmpos *pos = setup_pos_struct(XPOS, YPOS, WIDTH, HEIGHT);
    struct glmcolor *color = setup_color_struct(BG_WINDOW, FG_WINDOW, BG_MENU, FG_MENU);
    init_usermenu_root(gui, pos, color);
    
    // Display the username menu
    gtk_widget_show(gui->extra);
    gtk_widget_show(gui->widg);
    gtk_widget_show(gui->win);
}
