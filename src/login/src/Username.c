// 
// CONTRIBUTORS: 
// 
//     * Gabriel Gonzalez (gabeg@bu.edu) 
// 
// 
// LICENSE: 
// 
//     The MIT License (MIT)
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
// OPTIONS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     setup_menu             - Setup user menu.
//     setup_label            - Setup user menu label.
// 
//     usermenu_write_to_file - Write username to file.
// 
//     get_usernames          - Get username/uid combination from the specified file.
// 
//     set_username_entries   - Set entries in the user menu.
// 
//     display_icon           - Display username icon.
//     display_username       - Display the username menu.
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Setup Username Menu
//     * Write User Menu Item to File
//     * Get All Users On System
//     * Add Username Entries to the Menu
//     * Display Username 
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
//                          the config file, "set_config_pos", "set_config_txt", and
//                          "set_config_decor".
// 
//     gabeg Mar 19 2015 <> Utilized the universal setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
// 
//     gabeg Mar 21 2015 <> Altered the pango foreground color to auto-scale by 256
//                          so that the user does not have to. Note: The pango color
//                          color is strange.
// 
//     gabeg Mar 25 2015 <> No longer have to malloc for the application txt struct.
// 
//     gabeg Apr 04 2015 <> Now save current user in config file instead of a file 
//                          specific to the username. This means I can use the
//                          "read_config_char" function to get the username.
//                          
//                          Moved the system username sorter into the function that
//                          returns all the usernames on the system. Now a simple 
//                          loop is all it takes to add the user radio buttons.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Username.h"

// Declares
static void setup_menu(struct elyapp app);
static void setup_label(GtkWidget *label, struct elytxt txt);
static void usermenu_write_to_file(GtkMenu *item, GtkWidget *label);
static char ** get_usernames(char *file);
static void set_username_entries(GtkWidget *menu, GtkWidget *label);
static void display_icon();
static void display_usr_menu();



// ///////////////////////////////
// ///// SETUP USERNAME MENU /////
// ///////////////////////////////

// Setup dropdown menu that displays users
static void setup_menu(struct elyapp app) {
    
    double bmtime = benchmark_runtime(0);
    
    // Define label and menu widgets
    GtkWidget *label = gtk_label_new("");
    GtkWidget *menu = gtk_menu_new();
    
    // Set username menu attributes
    set_username_entries(menu, label);
    setup_label(label, app.txt);
    
    // Attach the window manager menu to the dropdown menu
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(app.widg), menu);
    gtk_container_add(GTK_CONTAINER(app.widg), label);
    gtk_button_set_relief(GTK_BUTTON(app.widg), GTK_RELIEF_NORMAL);
    
    // Display the label
    gtk_widget_show(label);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// Setup the label for the dropdown menu
static void setup_label(GtkWidget *label, struct elytxt txt) {
    
    double bmtime = benchmark_runtime(0);
    
    // Define text attributes
    PangoAttrList *attrList = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(txt.font);
    PangoAttribute *attrSize = pango_attr_size_new( (long)1024 * txt.size );
    PangoAttribute *attrColor = pango_attr_foreground_new( (long)256 * txt.red, 
                                                           (long)256 * txt.green, 
                                                           (long)256 * txt.blue);
    
    // Add attributes to the list (and increase the reference counter)
    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrColor);

    // Set label text
    gtk_label_set_text(GTK_LABEL(label), USERNAME);
    gtk_label_set_attributes(GTK_LABEL(label), attrList);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// ////////////////////////////////////////
// ///// WRITE USER MENU ITEM TO FILE /////
// ////////////////////////////////////////

// Write to a file, which user to login as
static void usermenu_write_to_file(GtkMenu *item, GtkWidget *label) {
    
    double bmtime = benchmark_runtime(0);
    
    USERNAME = (char*) gtk_menu_item_get_label( GTK_MENU_ITEM(item) );
    file_line_overwrite(USER_CONFIG, "user", USERNAME);
    
    gtk_label_set_text(GTK_LABEL(label), USERNAME);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// ///////////////////////////////////
// ///// GET ALL USERS ON SYSTEM /////
// ///////////////////////////////////

// Get username/uid combination from the specified file
static char ** get_usernames(char *file) {
    
    double bmtime = benchmark_runtime(0);
    
    // Open file for reading
    FILE *handle = fopen(file, "r");
    char line[MAX_CMD_LEN];
    
    // Available system users that can be signed into
    char container[MAX_NUM_LEN][MAX_STR_LEN];
    char sep = ':';
    int count = 0;
    
    // Loop through file lines
    while ( fgets(line, sizeof(line), handle) != NULL ) {
        
        // System user UID
        char uid_str[MAX_NUM_LEN];
        get_substring(uid_str, line, sep, 3);
        int uid = atoi(uid_str);
        
        // When valid UID value, add user to available user list
        if ( (uid == 0) || ((uid >= 1000) && (uid <= 9999)) ) {
            char user[MAX_STR_LEN];
            get_substring(user, line, sep, 1);
            snprintf(container[count], strlen(user)+1, "%s", user);
            
            ++count;
        }
    }
    
    // No users found in file
    if ( count == 0 )
        return NULL;
    
    // Allocate necessary space to hold all users
    char **ret = malloc( sizeof(*ret) * count);
    int i  = 0,
        loc    = 0;
    bool add   = false;
    
    // Add users to return array (from previous available user list)
    while (1) {
        
        // Compare label with the focus label
        add = true;
        
        if ( (strcmp(USERNAME, "User") == 0) 
             || (strcmp(container[loc], USERNAME) == 0) ) 
            ;
        else if ( (i != 0) 
                  && (container[loc][0] != '\0') ) 
            ;
        else
            add = false;
        
        // Setup the menu items
        if ( add == true ) {
            ret[i] = malloc(MAX_STR_LEN);
            snprintf(ret[i], strlen(container[loc])+1, "%s", container[loc]);
            container[loc][0] = '\0';
            
            // Increment and check array index values
            ++i;
            if ( i == count )
                break;
        }
        
        // Increment and check username add counter
        ++loc;
        if ( loc >= count )
            loc = 0;
    }
    
    // Null terminate the last element
    ret[i] = NULL;
    
    // Close file
    fclose(handle);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
    
    return ret;
}



// ////////////////////////////////////////////
// ///// ADD USERNAME ENTRIES TO THE MENU /////
// ////////////////////////////////////////////

// Set user name entries
static void set_username_entries(GtkWidget *menu, GtkWidget *label) {
    
    double bmtime = benchmark_runtime(0);
    
    // Initialize user menu items
    GtkWidget *user;
    GSList *group = NULL;
    
    // Search for login users in passwd files
    char *files[] = {"/etc/passwd", "/etc/shadow", NULL};
    
    // Set usernames from file
    char **allusers = get_usernames(files[0]);
    
    // Wrong file was used above, use the other file
    if ( allusers == NULL )
        allusers = get_usernames(files[1]);
    
    // Define menu item counters
    int i = 0;
    
    while ( allusers[i] != NULL ) {
        
        // Create the radio button menu label
        if ( i == 0 )
            user = gtk_radio_menu_item_new_with_label(NULL, allusers[i]);
        else
            user = gtk_radio_menu_item_new_with_label(group, allusers[i]);
        
        // Create the menu radio button group
        group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(user));
        
        // Attach window manager entries to the menu
        gtk_menu_attach(GTK_MENU(menu), user, 0, 1, i, i+1);
        gtk_widget_show(user);
        g_signal_connect(G_OBJECT(user), "activate", G_CALLBACK(usermenu_write_to_file), label);
        
        // Free up used memory
        free(allusers[i]);
        allusers[i] = NULL;
        
        ++i;
    }
    
    // Free the memory
    free(allusers);
    allusers = NULL;
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// ////////////////////////////
// ///// DISPLAY USERNAME /////
// ////////////////////////////

// Display username icon
static void display_icon() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Displaying Username icon...", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    // Allocate space for the application 
    struct elyapp app;
    
    // Define the application widget
    app.win  = gtk_window_new(GTK_WINDOW_POPUP);
    app.widg = gtk_image_new();
    
    // Create the username icon
    setup_app(USER_IMG_CONFIG, &app, NULL, NULL);
    gtk_image_set_from_file(GTK_IMAGE(app.widg), app.decor.img_file);
    
    // Log function completion
    if ( VERBOSE )
        file_log("Done\n");
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// Display the username menu
static void display_usr_menu() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Displaying Username menu button...", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    // Define default username
    USERNAME = read_config_char(USER_CONFIG, "user", MAX_STR_LEN);
    
    if ( USERNAME == NULL ) 
        USERNAME = "User";
    
    // Allocate space for the application 
    struct elyapp app;
    
    // Define the application widget
    app.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    app.widg = gtk_menu_button_new();
    
    // Setup the widget
    setup_app(USER_CONFIG, &app, NULL, NULL);
    setup_menu(app);
    
    // Log function completion
    if ( VERBOSE )
        file_log("Done\n");
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}


// Display username bundle
void display_username() {
    display_icon();
    display_usr_menu();
}
