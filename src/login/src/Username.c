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
//     gabeg Nov 01 2014 <> Changed the GTK window type for the username icon from
//                          TOPLEVEL to POPUP. Also changed "command_line" return
//                          value to be (char *) so altered code to reflect that
//                          change.
// 
//     gabeg Nov 08 2014 <> Made the username button have "relief". Changed the
//                          relief value from GTK_RELIEF_NONE to GTK_RELIEF_NORMAL
// 
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file. Included the new functions that read in from
//                          the preferences file, "set_pref_pos", "set_pref_txt", and
//                          "set_pref_decor".
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Username.h"

// Declares
static void display_icon();
static void setup_menu(GtkWidget *widg, GtkWidget *label);
static void setup_label(GtkWidget *label);
static void usermenu_write_to_file(GtkMenu *item, GtkWidget *label);
static char ** get_username(char *file, int size);
static void set_username_entries(GtkWidget *menu, GtkWidget *label);

static struct glmpos USR_POS;
static struct glmtxt USR_TXT;
static struct glmdecor USR_DECOR;

static struct glmpos IMG_POS;
static struct glmdecor IMG_DECOR;



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
    gtk_button_set_relief(GTK_BUTTON(widg), GTK_RELIEF_NORMAL);
}



// Setup the label for the dropdown menu
static void setup_label(GtkWidget *label) {
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(USR_TXT.font);
    PangoAttribute *attrSize = pango_attr_size_new(USR_TXT.size);
    PangoAttribute *attrColor = pango_attr_foreground_new(USR_TXT.red, USR_TXT.green, USR_TXT.blue);
    
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
    
    // Allocate memory for strings
    IMG_DECOR.img_file = malloc(READ_CHAR_LEN);
    
    // Define values from preference file
    set_pref_pos(USER_IMG_PREF, &IMG_POS);
    set_pref_decor(USER_IMG_PREF, &IMG_DECOR);
    
    // Initialize username icon widget elements
    GtkWidget *win = gtk_window_new(GTK_WINDOW_POPUP);
    GtkWidget *widg = gtk_image_new_from_file(IMG_DECOR.img_file);
    
    // Setup icon widget
    set_widget_color(win, widg, IMG_DECOR);
    setup_widget(win, widg, IMG_POS);
    
    // Display icon widget
    gtk_widget_show(widg);
    gtk_widget_show(win);
}



// Display the username menu
void display_username() {
    
    // Allocate memory for strings
    USR_TXT.font       = malloc(READ_CHAR_LEN);
    USR_DECOR.img_file = malloc(READ_CHAR_LEN);
    
    // Define values from preference file
    set_pref_pos(USER_PREF, &USR_POS);
    set_pref_txt(USER_PREF, &USR_TXT);
    set_pref_decor(USER_PREF, &USR_DECOR);
    
    // Define username
    USERNAME = file_read(USERNAME_LOG, 1, 20);
    
    // Initialize username menu elements
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *widg = gtk_menu_button_new();
    GtkWidget *label = gtk_label_new("");
    
    // Display username icon
    display_icon();
    
    // Setup username menu widget
    set_widget_color(win, widg, USR_DECOR);
    setup_widget(win, widg, USR_POS);
    setup_menu(widg, label);
    
    // Display the username menu
    gtk_widget_show(label);
    gtk_widget_show(widg);
    gtk_widget_show(win);
}
