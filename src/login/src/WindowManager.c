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
//         $ gcc -o WindowManager WindowManager.c Transparency.c `pkg-config gtk+-3.0 --cflags --libs`
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
//     command_line     - Return output of a Linux command
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
//     * Get Linux Command Output
//     * Add WM Entries to the Menu 
// 
// 
// Modification History:
// 	
//     gabeg Aug 02 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
// 
// **********************************************************************************


// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/WindowManager.h"
#include "../hdr/Transparency.h"
#include "../hdr/FileRW.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define   XPOS       770
#define   YPOS       315
#define   WIDTH      30
#define   HEIGHT     30
#define   IMG_FILE   "/etc/X11/glm/img/interface/settings.png"
#define   SES_FILE   "/etc/X11/glm/log/session.log"


// Declares
void init_wm_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu);
void wm_write_to_file(GtkMenu *item);
char * command_line(char *cmd);
void set_wm_entries(GtkWidget *menu);



// ////////////////////////////////////////////
// ///// INITIALIZE WINDOW MANAGER BUTTON /////
// ////////////////////////////////////////////

// Initialize the root window and its objects
void init_wm_root(GtkWidget *window, GtkWidget *dropmenu, GtkWidget *menu) {
    
    // Set window attributes
    gtk_window_set_title(GTK_WINDOW(window), "Window Manager");
    gtk_window_move(GTK_WINDOW(window), XPOS, YPOS);
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH*0, HEIGHT*0);
    
    // Define and set color schemes
    const GdkRGBA bg_widget = {0, 0, 0, 0};
    const GdkRGBA fg_widget = {1, 1, 1, 1};
    set_color_and_opacity(window, dropmenu, bg_widget, fg_widget);
    
    // Modify button style
    GtkWidget *image = gtk_image_new_from_file(IMG_FILE);
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
    file_write(SES_FILE, (char *)sess, "%s\n");
}



// ////////////////////////////////////
// ///// GET LINUX COMMAND OUTPUT ///// 
// ////////////////////////////////////

// Return command output as a string
char * command_line(char *cmd) {
    
    // Max character length of output string
    int outlen = 1024;
    char temp[outlen];
    char output[outlen];
    FILE *fp;
    
    strcpy(output, "");
    
    // Read command output
    fp = popen(cmd, "r");
    while (fgets(temp, sizeof(output)-1, fp) != NULL ) {
        
        // Remove trailing newline characters
        char *pos;
        if ((pos=strchr(temp, '\n')) != NULL)
            *pos = ' ';        
        
        // Append output to sessions string
        strcat(output, temp);
    }
    
    pclose(fp);
    
    
    // Allocate memory for command output 
    size_t sz = strlen(output);
    char *type = malloc(sz);  
    strncpy(type, output, sz);
    
    return(type);
}



// //////////////////////////////////////
// ///// ADD WM ENTRIES TO THE MENU /////
// //////////////////////////////////////

// Determine which window manager(s) the system has and add them as entries to the menu
void set_wm_entries(GtkWidget *menu) {
    
    // Get window manager string
    char *val = command_line("ls -1 /usr/share/xsessions/ | wc -l");
    char *wmstr = command_line("ls -1 /usr/share/xsessions/ | sed 's/.desktop//'");
    char *wmdup = strdup(wmstr);
    
    int num = atoi(val);
    char *allwm[num];
    
    
    // Set window manager array items
    int i = 0;
    char *buffer;
    while ( (buffer = strsep(&wmdup, " ")) ) {
        if ( strcmp(buffer, "") != 0 )
            allwm[i] = buffer;
        i++;
    }
    
    // Read previous window manager used
    char *wmfocus = file_read(SES_FILE);
    
    // Set the menu items
    int j = 0, q = 0, p = 0;
    GtkWidget *session;
    GSList *group;
    while (1) {
        if ( strcmp(allwm[j], wmfocus) == 0 ) {
            session = gtk_radio_menu_item_new_with_label(NULL, allwm[j]);
            group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(session));
            
            allwm[j] = "";
            p = 1;
        } else if ( q != 0 && strcmp(allwm[j], "") != 0 ) {
            session = gtk_radio_menu_item_new_with_label(group, allwm[j]);
            allwm[j] = "";
            p = 1;
        } else {
            p = 0;
        }
        
        // Setup the menu items
        if ( p == 1 ) {
            gtk_menu_attach(GTK_MENU(menu), session, 0, 1, q, q+1);
            gtk_widget_show(session);
            g_signal_connect(G_OBJECT(session), "activate", G_CALLBACK(wm_write_to_file), NULL);
            
            q++;
            if ( q == num ) 
                break;
        }
        
        
        j++;
        if ( j >= num )
            j = 0;
    }
    
    
    // Freeing up the memory
    free(val);
    free(wmstr);
    free(wmfocus);
}
