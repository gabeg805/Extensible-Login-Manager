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
//     WindowManager.c
// 
// 
// SYNTAX: 
// 
//     Include the header file:
// 
//         #include "WindowManager.h"
// 
// 
// PURPOSE:
// 
//     Display a button for the user to choose between possible window managers for 
//     their login session.
// 
// 
// OPTIONS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     setup_menu                 - Setup window manager dropdown menu.
// 
//     wm_write_to_file           - Write to a file, which window manager to use for the 
//                                  session.
// 
//     get_window_managers        - Return the window managers that are on the system.
//     set_window_manager_entries - Add the window managers onto the WM menu.
// 
//     display_window_manager     - Display the window manager button.
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Setup Window Manager Menu
//     * Write Window Manager to File
//     * Add WM Entries to the Menu 
//     * Display Window Manager
// 
// 
// MODIFICATION HISTORY:
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
//     gabeg Nov 08 2014 <> Moved the position of the window manager button 
// 
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file. Included the new functions that read in from
//                          the config file, "set_config_pos" and 
//                          "set_config_decor".
// 
//     gabeg Mar 19 2015 <> Utilized the universal setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
// 
//     gabeg Apr 04 2015 <> Now save current window manager in config file instead of 
//                           a file specific to the window manager. This means I can
//                           use the "read_config_char" function to get the window
//                           manager.
//                           
//                           Removed the #define command in the header file. Moved it
//                           to the config file for easier access to the user. 
//                           
//                           Added a function to return all window managers on the 
//                           system. It sorts them, and then setting the window
//                           manager radio buttons is a simple loop.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "WindowManager.h"
#include "elysia.h"

// Declares
static void setup_menu(struct elyapp app);
static void wm_write_to_file(GtkMenu *item);
static char ** get_window_managers();
static void set_window_manager_entries(GtkWidget *menu);



// /////////////////////////////////////
// ///// SETUP WINDOW MANAGER MENU /////
// /////////////////////////////////////

// Setup window manager dropdown menu button
static void setup_menu(struct elyapp app) {
    
    double bmtime = benchmark_runtime(0);
    
    // Set entries in window manager dropdown menu
    GtkWidget *menu = gtk_menu_new();
    set_window_manager_entries(menu);
    
    // Modify button style
    GtkWidget *icon = gtk_image_new_from_file(app.decor.img_file);
    gtk_button_set_image(GTK_BUTTON(app.widg), icon);
    gtk_button_set_relief(GTK_BUTTON(app.widg), GTK_RELIEF_NONE);
    
    // Make menu popup
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(app.widg), menu);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// ////////////////////////////////////////
// ///// WRITE WINDOW MANAGER TO FILE ///// 
// ////////////////////////////////////////

// Write to a file, which window manager to use for the session
static void wm_write_to_file(GtkMenu *item) {
    
    double bmtime = benchmark_runtime(0);
    
    SESSION = (char *) gtk_menu_item_get_label( GTK_MENU_ITEM(item) );
    file_line_overwrite(WM_CONFIG, "wm", SESSION);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// //////////////////////////////////////
// ///// ADD WM ENTRIES TO THE MENU /////
// //////////////////////////////////////

// Return all the window managers on the system
static char ** get_window_managers() {
    
    double bmtime = benchmark_runtime(0);
    
    // Execute command to get system window managers
    char *cmd = read_config_char(WM_CONFIG, "cmd", MAX_CMD_LEN);
    char arr[MAX_CMD_LEN];
    get_cmd_output(arr, sizeof(arr), cmd);
    
    // Initialize array indicies
    int i   = 0,
        j   = 0,
        loc = 0;
    
    // Count number of window managers
    char sep  = '\n';
    int count = 0;
    
    while ( arr[i] != '\0' ) {
        if ( arr[i] == sep )
            ++count;
        ++i;
    }
    
    // No window managers found
    if ( count == 0 )
        return NULL;
    
    // Initialize array containing all window managers
    char container[count+1][MAX_STR_LEN];
    memset(container[0], 0, MAX_STR_LEN);
    
    // Package window managers into one array
    i   = 0;
    j   = 0;
    loc = 0;
    
    while ( arr[i] != '\0' ) {
        if ( arr[i] != '\n' ) {
            container[loc][j] = arr[i];
            ++j;
        } else {
            j = 0;
            ++loc;
            memset(container[loc], 0, MAX_STR_LEN);
        }
        
        ++i;
    }
    
    // Allocate necessary space to hold all users
    char **ret = malloc( sizeof(*ret) * count);
    bool add   = false;
    i   = 0;
    loc = 0;
    
    // Add users to return array (from previous available user list)
    while (1) {
        
        // Compare label with the focus label
        add = true;
        
        if ( (strcmp(SESSION, "xterm") == 0) 
             || (strcmp(container[loc], SESSION) == 0) ) 
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
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
    
    return ret;
}



// Determine which window manager(s) the system has and add them as entries to the menu
static void set_window_manager_entries(GtkWidget *menu) {
    
    double bmtime = benchmark_runtime(0);
    
    // Initialize WM session items
    GtkWidget *sesh;
    GSList *group = NULL;
    
    // Get all window managers in one string 
    char **allwm = get_window_managers();
    
    // Create menu labels based on the window managers that were found
    int i = 0;
    
    while ( allwm[i] != NULL ) {
        
        // Create the radio buttons for the window managers
        if ( i == 0 )
            sesh = gtk_radio_menu_item_new_with_label(NULL, allwm[i]);
        else
            sesh = gtk_radio_menu_item_new_with_label(group, allwm[i]);
        
        // Create the group for the radio buttons
        group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(sesh));
        
        // Attach radio buttons to the menu
        gtk_menu_attach(GTK_MENU(menu), sesh, 0, 1, i, i+1);
        gtk_widget_show(sesh);
        g_signal_connect(G_OBJECT(sesh), "activate", G_CALLBACK(wm_write_to_file), NULL);
        
        // Free up used memory
        free(allwm[i]);
        allwm[i] = NULL;
        
        ++i;
    }
    
    // Free final piece of memory
    free(allwm);
    allwm = NULL;
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// //////////////////////////////////
// ///// DISPLAY WINDOW MANAGER /////
// //////////////////////////////////

// Display the window manager button
void display_window_manager() {
    
    double bmtime = benchmark_runtime(0);
    
    // Log function start
    if ( VERBOSE )
        file_log("%s: (%s:%d): Displaying window manager menu button...", 
                 __FILE__, __FUNCTION__, __LINE__);
    
    // Define session
    SESSION = read_config_char(WM_CONFIG, "wm", MAX_STR_LEN);
    
    if ( SESSION == NULL )
        SESSION = "xterm";
    
    // Allocate space for application 
    static struct elyapp app;
    
    // Define the application widget
    app.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    app.widg = gtk_menu_button_new();
    
    // Create the window manager menu button
    setup_app(WM_CONFIG, &app, NULL, NULL);
    setup_menu(app);
    
    // Log function completion
    if ( VERBOSE )
        file_log("Done\n");
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}
