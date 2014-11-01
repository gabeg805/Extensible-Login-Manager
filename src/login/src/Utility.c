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
//     count_char          - Count number of times a character occurs in a string
// 
//     get_open_tty        - Return an open TTY port
// 
//     get_open_display    - Search for X display lock files and return a display 
//                           that is not locked
// 
//     file_write          - Write to a file
// 
//     file_read           - Read a file's contents
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
//     * Character Count In String
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
//     gabeg Oct 31 2014 <> Modified "get_open_display", "file_read", and 
//                          "command_line".  In "get_open_display", instead of
//                          returning a (char *), I return the int value of the
//                          display. In "file_read" I made it so that you can specify
//                          a file line to return.  In "command_line", I removed all
//                          the calls to malloc and just made it return all the
//                          output as one string. In both modified functions, instead
//                          of returning a (char *), I return void and have the user
//                          input a (char *) to be modified. That way malloc does not
//                          have to be called. I also added a function "count_char"
//                          which counts the number of times a character occurs in a
//                          string.
// 
//     gabeg Nov 01 2014 <> Testing to see if calls to malloc are expensive by
//                          changing "file_read" and "command_line" to return 
//                          (char *). It seems they're not that expensive so I'm
//                          keeping them as returning as returning (char *).
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
int count_char(char *str, char val);
int get_open_tty();
int get_open_display();
void file_write(char *file, char *opt, const char *fmt, ...);
char * file_read(char *file, int ln, int sz);
char * command_line(char *cmd, size_t sz, size_t sza);
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



// /////////////////////////////////////
// ///// CHARACTER COUNT IN STRING /////
// /////////////////////////////////////

// Count number of times a character occurs in a string
int count_char(char *str, char val) {
    int i = 0;
    int count = 0;
    int len = strlen(str);
    
    while ( i < len ) {
        if ( str[i] == val )
            ++count;
        ++i;
    }
    
    return count;
}



// ////////////////////////
// ///// GET OPEN TTY /////
// ////////////////////////

// Return an open TTY port
// * Find a better way to do this
int get_open_tty() {
    
    // Look for running process that opens TTY
    /* char *cmd = "pgrep -a tty | grep -o -E 'tty. '"; */
    /* char tty[50]; */
    /* command_line(cmd, sizeof(tty), sizeof(tty), tty); */
    /* int num = atoi(tty); */
    
    /* // Loop through all process command strings */
    /* int n = 1, i = 1; */
    /* while ( (num != 0) && (i <= num) ) { */
    /*     char *sep; */
    /*     char *buffer, *orig; */
    /*     buffer = orig = strdup(tty[i]); */
        
    /*     while ( (sep = strsep(&buffer, " ")) != NULL ) { */
            
    /*         // Check process command for 'tty#' */
    /*         char *ttyrun = strstr(sep, "tty"); */
    /*         char *ttycmd = strstr(sep, "getty"); */
            
    /*         if ( (ttyrun != NULL) && (ttycmd == NULL) )  */
    /*             n = atoi(&sep[3])+1; */
    /*     } */
        
    /*     free(orig); */
    /*     free(tty[i]); */
    /*     i++; */
    /* } */
    
    /* free(tty[0]); */
    /* free(tty); */
    int n = 7;
    return n;
}



// //////////////////////////////
// ///// GET OPEN X DISPLAY ///// 
// //////////////////////////////

// Return an open display in the form ':0'
int get_open_display() {
    
    // Loop through possible displays
    int d;
    char filename[15];
    /* char display[5]; */
    
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
    
    return d;
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
char * file_read(char *file, int ln, int sz) {
    
    // Store file contents in variable
    FILE *handle = fopen(file, "r");
    char temp[sz];
    
    // Loop through file
    int i = 0;
    while (fgets(temp, sz, handle) != NULL ) {
        
        // Remove trailing newline characters
        char *pos;
        if ((pos=strchr(temp, '\n')) != NULL)
            *pos = '\0';
        
        // Break out of loop
        ++i;
        if ( i == ln )
            break;
    }
    
    // Close file
    fclose(handle);
    
    // Copy the line int to the return variable
    size_t len = strlen(temp) + 1;
    char *copy = malloc(len);
    assert(copy);
    snprintf(copy, len, "%s", temp);
    
    return copy;
}



// ////////////////////////////////////
// ///// GET LINUX COMMAND OUTPUT ///// 
// ////////////////////////////////////

// Store command output as a string inside variable
char * command_line(char *cmd, size_t sz, size_t sza) { 
    
    // Output arrays
    FILE *handle  = popen(cmd, "r");
    char temp[sz];
    char contents[sza];
    strcpy(contents, "");
    
    // Add file contents to contents variable
    while (fgets(temp, sz, handle) != NULL ) {
        int lc = strlen(contents);
        int lt = strlen(temp);
        if ( sza >  (lc+lt) )
            strcat(contents, temp);
    }
    
    // Close process
    pclose(handle);
    
    // Copy file contents to the return variable
    size_t len = strlen(contents) + 1;
    char *copy = malloc(len);
    assert(copy);
    snprintf(copy, len, "%s", contents);
    
    return copy;
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
