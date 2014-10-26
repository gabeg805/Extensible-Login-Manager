// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Utility.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the following header:
// 
//         #include "Utility.h"
// 
// 
// PURPOSE:
// 
//     Contains various functions that are used frequently.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     cleanup_child       - Remove zombie processes
// 
//     get_open_tty        - Return an open TTY port
// 
//     file_write          - Write to a file
// 
//     file_read           - Read a file's contents
// 
//     get_open_display    - Search for X display lock files and return a display 
//                           that is not locked
// 
//     command_line        - Return output of a Linux command
// 
//     enable_transparency - Enable widget transparency
// 
//     set_widget_color    - Set a widget's color and opacity 
// 
//     setup_widget        - Setup widget and window
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Remove Zombie Processes
//     * Get Open TTY 
//     * Write to File
//     * Read File
//     * Get Open X Display
//     * Get Linux Command Output
//     * Enable Widget Transparency
//     * Set Widget Color and Opacity
//     * Setup Widget
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 12 2014 <> created
// 
//     gabeg Aug 14 2014 <> Modified command_line function to return an array of 
//                          strings instead of just one long string
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries, and added "cleanup_child"
// 
//     gabeg Oct 25 2014 <> Moved "Transparency" source file functions here and 
//                          removed the "Transparency" source file. Added the 
//                          universal widget setup function.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


//Declares
void cleanup_child(int signal);
int get_open_tty();
void file_write(char *file, char *opt, const char *fmt, ...);
char * file_read(char *file);
char * get_open_display();
char ** command_line(char *cmd, int size);
void enable_transparency(GtkWidget *widget);
void set_widget_color(GtkWidget *win, GtkWidget *widg, const GdkRGBA color[4]);
void setup_widget(GtkWidget *win, GtkWidget *widg, int pos[4], const GdkRGBA color[4]);



// ///////////////////////////////////
// ///// REMOVE ZOMBIE PROCESSES /////
// ///////////////////////////////////

// Clean up child zombie process
void cleanup_child(int signal) {
    wait(NULL);
}



// ////////////////////////
// ///// GET OPEN TTY /////
// ////////////////////////

// Return an open TTY port
int get_open_tty() {
    
    // Look for running process that opens TTY
    char *cmd = "pgrep -a tty";
    char **tty = command_line(cmd, 50);
    int num = atoi(tty[0]);
    
    // Loop through all process command strings
    int n = 1, i = 1;
    while ( num != 0 && i <= num ) {
        char *sep;
        char *buffer, *orig;
        buffer = orig = strdup(tty[i]);
        
        while ( (sep = strsep(&buffer, " ")) != NULL ) {
            
            // Check process command for 'tty#'
            char *ttyrun = strstr(sep, "tty");
            char *ttycmd = strstr(sep, "getty");
            
            if ( (ttyrun != NULL) && (ttycmd == NULL) ) {
                char val[3];
                snprintf(val, 3, "%c", sep[3]);
                n = atoi(val)+1;
            }
        }
        
        free(orig);
        free(tty[i]);
        i++;
    }
    
    free(tty[0]);
    free(tty);
    n = 7;
    return n;
}



// /////////////////////////
// ///// WRITE TO FILE /////
// /////////////////////////

// Write to a file
void file_write(char *file, char *opt, const char *fmt, ...) {
    FILE *handle = fopen(file, opt);
    va_list args;
    
    va_start(args, fmt);
    vfprintf(handle, fmt, args);
    va_end(args);
    
    fclose(handle);
}



// /////////////////////
// ///// READ FILE /////
// /////////////////////

// Read a file's contents
char * file_read(char *file) {
    
    // Store file contents in variable
    FILE *handle = fopen(file, "r");
    char temp[100];
    
    fgets(temp, sizeof(temp), handle);
    fclose(handle);
    
    // Remove trailing newline characters
    char *pos;
    if ((pos=strchr(temp, '\n')) != NULL)
        *pos = '\0';        
    
    // Allocate memory for command output 
    size_t sz = strlen(temp) + 1;
    char *contents = malloc(sz);
    assert(contents);
    snprintf(contents, sz, temp);
    
    return contents;
}



// //////////////////////////////
// ///// GET OPEN X DISPLAY ///// 
// //////////////////////////////

// Return an open display in the form ':0'
char * get_open_display() {
    
    // Loop through possible displays
    int d;
    char filename[15];
    char display[5];
    
    for ( d=0; d < 10; d++ ) {
        
        // Pieces of the actual file name
        char *xtmp = "/tmp/.X";
        char *xlock = "-lock";
        snprintf(filename, sizeof(filename), "%s%d%s", xtmp, d, xlock);
        
        // Check for file existence
        int result = access(filename, F_OK);
        
        if ( result != 0 ) 
            break;
    }
    
    // Define open display
    snprintf(display, sizeof(display), ":%d", d);
    
    // Allocate memory for display output 
    size_t sz = strlen(display) + 1;
    char *opendisp = malloc(sz);
    assert(opendisp);
    snprintf(opendisp, sz, display);
    
    return opendisp;
}



// ////////////////////////////////////
// ///// GET LINUX COMMAND OUTPUT ///// 
// ////////////////////////////////////

// Return command output as a string
char ** command_line(char *cmd, int size) {
    
    // Output arrays
    char **array = malloc(sizeof(char*)*size);
    assert(array);
    char temp[size];
    array[0] = "0";
    
    // Read command output
    FILE *fp  = popen(cmd, "r");
    int i = 1;
    while (fgets(temp, sizeof(temp), fp) != NULL ) {
        
        // Remove trailing newline characters
        char *pos;
        if ((pos=strchr(temp, '\n')) != NULL)
            *pos = '\0';
        
        // Add string to the array
        size_t sz = strlen(temp);
        char *copy = malloc(sz+1);
        assert(copy);
        snprintf(copy, sz+1, temp);
        
        array[i] = copy;
        ++i;
    }
    
    // Close process
    pclose(fp);
    
    // Put number of array elements into first index
    char val[5];
    snprintf(val, sizeof(val), "%d", (i-1));
    
    size_t sz = strlen(val) + 1;
    char *copy = malloc(sz);
    assert(copy);
    snprintf(copy, sz, val);
    
    array[0] = copy;
    
    return array;
}



// //////////////////////////////////////
// ///// ENABLE WIDGET TRANSPARENCY /////
// //////////////////////////////////////

// Enable widget transparency
void enable_transparency(GtkWidget *widget) {
    
    // To check if the display supports alpha channels, get the visual 
    GdkScreen *screen = gtk_widget_get_screen(widget);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    
    // Set widget visual
    gtk_widget_set_visual(widget, visual);
}



// ////////////////////////////////////////
// ///// SET WIDGET COLOR AND OPACITY /////
// ////////////////////////////////////////

// Setup widget color
void set_widget_color(GtkWidget *win, GtkWidget *widg, const GdkRGBA color[4]) {
    
    // Set the color scheme
    gtk_widget_override_background_color(win, GTK_STATE_FLAG_NORMAL, &color[0]);
    gtk_widget_override_background_color(widg, GTK_STATE_FLAG_NORMAL, &color[1]);
    gtk_widget_override_color(win, GTK_STATE_FLAG_NORMAL, &color[2]);
    gtk_widget_override_color(widg, GTK_STATE_FLAG_NORMAL, &color[3]);
}



// ////////////////////////
// ///// SETUP WIDGET /////
// ////////////////////////

// Universal setup function
void setup_widget(GtkWidget *win, GtkWidget *widg, int pos[4], const GdkRGBA color[4]) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(win), pos[0], pos[1]);
    gtk_window_set_default_size(GTK_WINDOW(win), pos[2], pos[3]);
    
    // Set color scheme for root window
    if ( color != NULL )
        set_widget_color(win, widg, color);
    
    // Add entry to window
    gtk_container_add(GTK_CONTAINER(win), widg);
    
    // Enable transparency
    enable_transparency(win);
    
    // GTK signals
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);    
}
