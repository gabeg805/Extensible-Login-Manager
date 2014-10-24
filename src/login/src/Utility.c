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
//     Allows easy access to reading and writing to files.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     file_write       - Write to a file
// 
//     file_read        - Read a file's contents
// 
//     get_open_display - Search for X display lock files and return a display that 
//                        is not locked
// 
//     get_open_tty     - Return an open TTY port
// 
//     command_line     - Return output of a Linux command
// 
//     cleanup_child    - Remove zombie processes
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Write to File
//     * Read File
//     * Get Open X Display
//     * Get Open TTY 
//     * Get Linux Command Output
//     * Remove Zombie Processes
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
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Utility.h"
#include "../hdr/Config.h"
#include "../hdr/Transparency.h"

#include <gtk/gtk.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


//Declares
void file_write(char *file, char *opt, const char *fmt, ...);
char * file_read(char *file);
char * get_open_display();
int get_open_tty();
char ** command_line(char *cmd, int size);
void cleanup_child(int signal);
struct glmstruct * setup_struct(GtkWidget *window, GtkWidget *widget, 
                                struct glmpos *pos,
                                struct glmcolor *color,
                                struct glmtext *text);
struct glmgui * setup_gui_struct(GtkWidget *window, GtkWidget *widget, GtkWidget *extra, char *img);
struct glmpos * setup_pos_struct(int x, int y, int width, int height);
struct glmcolor * setup_color_struct(const GdkRGBA bg_window, const GdkRGBA fg_window, 
                                     const GdkRGBA bg_widget, const GdkRGBA fg_widget);
struct glmtext * setup_text_struct(GtkWidget *widg, char *font, char *fmt, int size);
void setters(GtkWidget *win, GtkWidget *widg, int pos[4], const GdkRGBA color[4]);


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
    snprintf(opendisp, sz, display);
    
    return opendisp;
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



// ////////////////////////////////////
// ///// GET LINUX COMMAND OUTPUT ///// 
// ////////////////////////////////////

// Return command output as a string
char ** command_line(char *cmd, int size) {
    
    // Output arrays
    char **array = malloc(sizeof(char*)*size);
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
    snprintf(copy, sz, val);
    
    array[0] = copy;
    
    return array;
}



// ///////////////////////////////////
// ///// REMOVE ZOMBIE PROCESSES /////
// ///////////////////////////////////

// Clean up child zombie process
void cleanup_child(int signal) {
    wait(NULL);
}



// ////////////////////////////
// ///// SETUP GUI STRUCT /////
// ////////////////////////////

// Setup the GUI struct
struct glmstruct * setup_struct(GtkWidget *window, GtkWidget *widget, 
                                struct glmpos *pos,
                                struct glmcolor *color,
                                struct glmtext *text) {
    
    // Initialize struct
    struct glmstruct *gui = malloc(sizeof(struct glmstruct));
    
    // Start storing things in the struct
    gui->win = window;
    gui->widg = widget;
    
    if ( pos != NULL )
        gui->pos = pos;
    
    if ( color != NULL )
        gui->color = color;
    
    if ( text != NULL )
        gui->text = text;
    
    return gui;
}



// Setup the GUI widgets struct
struct glmgui * setup_gui_struct(GtkWidget *window, GtkWidget *widget, GtkWidget *extra, char *img) {
    
    // Initialize struct
    struct glmgui *gui = malloc(sizeof(struct glmgui));
    
    // Start storing things in the struct
    gui->win = window;
    gui->widg = widget;
    
    if ( extra != NULL )
        gui->extra = extra;
    
    if ( img != NULL )
        gui->img = img;
    
    return gui;
}    
    
// Setup the GUI position struct
struct glmpos * setup_pos_struct(int x, int y, int width, int height) {
    
    struct glmpos *pos = malloc(sizeof(struct glmpos));
    
    pos->x = x;
    pos->y = y;
    pos->width = width;
    pos->height = height;
    
    return pos;
}



// Setup the GUI colors struct
struct glmcolor * setup_color_struct(const GdkRGBA bg_window, const GdkRGBA fg_window, 
                                     const GdkRGBA bg_widget, const GdkRGBA fg_widget) {
    
    struct glmcolor *color = malloc(sizeof(struct glmcolor));
    
    color->bgwin = bg_window;
    color->fgwin = fg_window;
    color->bgwidg = bg_widget;
    color->fgwidg = fg_widget;
    
    
    return color;
}



// Setup the GUI text struct
struct glmtext * setup_text_struct(GtkWidget *widg, char *font, char *fmt, int size) {
    
    struct glmtext *text = malloc(sizeof(struct glmtext));
    
    text->widg = widg;
    text->font = font;
    text->fmt = fmt;
    text->size = size;
    
    return text;
}



// Universal setup function
void setters(GtkWidget *win, GtkWidget *widg, int pos[4], const GdkRGBA color[4]) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(win), pos[0], pos[1]);
    gtk_window_set_default_size(GTK_WINDOW(win), pos[2], pos[3]);
    
    // Set color scheme for root window
    if ( color != NULL )
        set_ass(win, widg, color);
    
    // Add entry to window
    gtk_container_add(GTK_CONTAINER(win), widg);
    
    // Enable transparency
    enable_transparency(win);
    
    // GTK signals
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);    
}
