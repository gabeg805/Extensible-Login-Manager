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
//         #include "../hdr/WindowManager.h"
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
//     setup_menu             - Setup window manager dropdown menu.
// 
//     wm_write_to_file       - Write to a file, which window manager to use for the 
//                              session.
// 
//     set_wm_entries         - Determine which window manager(s) the system has and add 
//                              them as entries to the menu.
// 
//     display_window_manager - Display the window manager button.
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
//                          the preferences file, "set_pref_pos" and 
//                          "set_pref_decor".
// 
//     gabeg Mar 19 2015 <> Utilized the universal setup function and also enabled 
//                          a method to have the application write verbosely to the  
//                          log, in the event that a problem arises.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/WindowManager.h"

// Declares
static void setup_menu(struct glmapp app);
static void wm_write_to_file(GtkMenu *item);
static void set_wm_entries(GtkWidget *menu);



// /////////////////////////////////////
// ///// SETUP WINDOW MANAGER MENU /////
// /////////////////////////////////////

// Setup window manager dropdown menu button
static void setup_menu(struct glmapp app) {
    
    double bmtime = benchmark_runtime(0);
    
    // Set entries in window manager dropdown menu
    GtkWidget *menu = gtk_menu_new();
    set_wm_entries(menu);
    
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
    
    const gchar *sess = gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    snprintf(SESSION, strlen(sess)+1, "%s", (char*)sess);
    file_write(SESSION_LOG, "w", "%s\n", SESSION);
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



// //////////////////////////////////////
// ///// ADD WM ENTRIES TO THE MENU /////
// //////////////////////////////////////

// Determine which window manager(s) the system has and add them as entries to the menu
static void set_wm_entries(GtkWidget *menu) {
    
    double bmtime = benchmark_runtime(0);
    
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
        } else if ( (q != 0) && (allwm[i] != NULL) ) {
            if ( strcmp(allwm[i], "") != 0 )
                sesh = gtk_radio_menu_item_new_with_label(group, allwm[i]);
        } else
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
    SESSION = file_read(SESSION_LOG, 1, 20);
    
    // Allocate space for application 
    static struct glmapp app;
    
    // Define the application widget
    app.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    app.widg = gtk_menu_button_new();
    
    // Create the window manager menu button
    setup_widget(WM_PREF, &app, NULL, NULL);
    setup_menu(app);
    
    // Log function completion
    if ( VERBOSE )
        file_log("Done\n");
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}
