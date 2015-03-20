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
//     cleanup_child       - Remove zombie processes.
// 
//     count_char          - Count number of times a character occurs in a string.
// 
//     file_write          - Write to a file.
// 
//     file_read           - Read a file's contents.
// 
//     command_line        - Return output of a Linux command.
// 
//     read_pref_char      - Read the preference file and output a string.
//     read_pref_int       - Read the preference file and output an int.
// 
//     set_pref_pos        - Set position values defined in the preference file.
//     set_pref_txt        - Set text values defined in the preference file.
//     set_pref_decor      - Set decoration values defined in the preference file.
// 
//     set_widget_pos      - Set the size and position of the widget.
//     set_widget_color    - Set the color and opacity of the widget.
//     enable_transparency - Enable widget transparency.
//     setup_app           - Setup the application.
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Remove Zombie Processes
//     * Character Count In String
//     * Write to File
//     * Read File
//     * Get Linux Command Output
//     * Read Preference File
//     * Set Preference Values
//     * Application Setup 
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
//     gabeg Mar 17 2015 <> Moved excess preprocessor calls and declarations into the
//                          header file. Also added new funtions that read in key 
//                          value pairs from a preferences file. In doing so I remove
//                          all the preprocessor define statements and just put all 
//                          the variables I need into glm structures. This hopefully 
//                          makes the code look cleaner.
// 
//     gabeg Mar 19 2015 <> Attempted to make code look cleaner by creating a 
//                          universal setup function for each application by 
//                          incorporating a struct 'glmapp' that holds everything an
//                          app will need.
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Utility.h"



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




// ////////////////////////////////
// ///// READ PREFERENCE FILE /////
// ////////////////////////////////

// Read the preference file and output a string
void read_pref_char(char *ret, int n, int m, char *file, char *key) {
    
    // File variables
    FILE *handle = fopen(file, "r");
    char temp[n];
    
    // Compare attribute name with length of key
    int count = 0,
        len = strlen(key);
    
    // Array indicies
    int i = 0,
        j = 0;
    
    // Loop through line by line the contents of the preference file
    while ( fgets(temp, sizeof(temp), handle) != NULL ) {
        
        // Reset to 0
        count = 0;
        i = 0; 
        j = 0;
        
        bool sep = false,
            found = false;
        
        // Loop through each char of the line
        while ( temp[i] != 0 ) {
            
            // Compare attribute name to given key
            if ( !sep || !found ) {
                
                if ( temp[i] == ':' )
                    sep = true;
                
                if ( !found && (i < len) && (temp[i] == key[i]) )
                    ++count;
                
                if ( count == len )
                    found = true;
                
            } else {
                
                // Attribute found, store value of attribute
                if ( (j == 0) && (temp[i] == ' ') )
                    ;
                else {
                    // Keep space for null terminater and ignore newlines
                    if ( (j < (m-1)) && (temp[i] != '\n') ) {
                        
                        // If match found then clear memory
                        if ( j == 0 )
                            memset(ret, 0, m);
                        
                        // Add char to the return array
                        ret[j] = temp[i];
                        ++j;
                    }
                }
            }
            
            ++i;
        }
        
        // Attribute was found so return the value
        if ( found )
            break;
    }
    
    // Close the file
    fclose(handle);
    
    // Set default value to 0 if key not found
    if ( j == 0 )
        ret = NULL;
}



// Read preference file and output an int
void read_pref_int(int *ret, int n, char *file, char *key) {
    
    // File variables
    FILE *handle = fopen(file, "r");
    char temp[n];
    
    // Compare attribute name with length of key
    int count = 0,
        len = strlen(key);
    
    // Array indicies
    int i = 0,
        j = 0;
    
    // Loop through line by line the contents of the preference file
    while ( fgets(temp, sizeof(temp), handle) != NULL ) {
        
        // Reset to 0
        count = 0;
        i = 0;
        j = 0;
        
        bool sep = false,
            found = false;
        
        // Loop through each char of the line
        while ( temp[i] != 0 ) {
            
            // Compare attribute name to given key
            if ( !sep || !found ) {
                
                if ( temp[i] == ':' )
                    sep = true;
                
                if ( !found && (i < len) && (temp[i] == key[i]) )
                    ++count;
                
                if ( count == len )
                    found = true;
                
            } else {
                
                // Attribute found, store value of attribute
                if ( (j == 0) && (temp[i] == ' ') )
                    ;
                else {
                    if ( temp[i] != '\n' ) {
                        if ( j == 0 )
                            *ret = 0;
                        
                        *ret = (*ret * 10) + (temp[i] - '0');
                        ++j;
                    }
                }
            }
            
            ++i;
        }
        
        // Attribute was found so return the value
        if ( found )
            break;
    }
    
    // Close the file
    fclose(handle);
    
    // Set default value to 0 if key not found
    if ( j == 0 )
        *ret = 0;
}



// /////////////////////////////////
// ///// SET PREFERENCE VALUES /////
// /////////////////////////////////

// Set preference position values
void set_pref_pos(char *file, struct glmpos *pos) {
    read_pref_int(&pos->x,      READ_INT_LEN, file, "xpos");
    read_pref_int(&pos->y,      READ_INT_LEN, file, "ypos");
    read_pref_int(&pos->width,  READ_INT_LEN, file, "width");
    read_pref_int(&pos->height, READ_INT_LEN, file, "height");
}



// Set preference text values
void set_pref_txt(char *file, struct glmtxt *txt) {
    read_pref_int(&txt->size,     READ_CHAR_LEN, file, "size");
    read_pref_int(&txt->maxchars, READ_CHAR_LEN, file, "maxchars");
    
    read_pref_char(txt->text,     READ_CHAR_LEN, READ_INT_LEN, file, "text");
    read_pref_char(txt->font,     READ_CHAR_LEN, READ_INT_LEN, file, "font");
    read_pref_char(txt->fmt,      READ_CHAR_LEN, READ_INT_LEN, file, "fmt");
    read_pref_char(&txt->invis,   READ_CHAR_LEN, READ_INT_LEN, file, "invis");
    
    read_pref_int(&txt->red,      READ_CHAR_LEN, file, "txt-red");
    read_pref_int(&txt->green,    READ_CHAR_LEN, file, "txt-green");
    read_pref_int(&txt->blue,     READ_CHAR_LEN, file, "txt-blue");
}



// Set preference decoration values
void set_pref_decor(char *file, struct glmdecor *decor) {
    read_pref_char(decor->img_file, READ_PATH_LEN, READ_CHAR_LEN, file, "img-file");
    
    read_pref_int(&decor->bg_red,   READ_INT_LEN,  file, "bg-red");
    read_pref_int(&decor->bg_green, READ_INT_LEN,  file, "bg-green");
    read_pref_int(&decor->bg_blue,  READ_INT_LEN,  file, "bg-blue");
    read_pref_int(&decor->bg_alpha, READ_INT_LEN,  file, "bg-alpha");
    
    read_pref_int(&decor->fg_red,   READ_INT_LEN,  file, "fg-red");
    read_pref_int(&decor->fg_green, READ_INT_LEN,  file, "fg-green");
    read_pref_int(&decor->fg_blue,  READ_INT_LEN,  file, "fg-blue");
    read_pref_int(&decor->fg_alpha, READ_INT_LEN,  file, "fg-alpha");
    
    read_pref_int(&decor->div,      READ_INT_LEN,  file, "div");
    
    // Set default value to 1
    if ( decor->div == 0 )
        decor->div = 1;
}



// /////////////////////////////
// ///// APPLICATION SETUP /////
// /////////////////////////////

// Set widget position 
void set_widget_pos(struct glmapp *app) {
    
    // Set window attributes
    gtk_window_move(GTK_WINDOW(app->win), app->pos.x, app->pos.y);
    gtk_window_set_default_size(GTK_WINDOW(app->win), app->pos.width, app->pos.height);
}



// Set the color of a widget 
void set_widget_color(struct glmapp *app) {
    
    /* if ( decor == NULL ) */
    /*     return; */
    
    // Define background and foreground window color
    const GdkRGBA bg_win = {0, 0, 0, 0};
    const GdkRGBA fg_win = {0, 0, 0, 0};
    
    // Define background and foreground widget color
    double bg_red = (double) app->decor.bg_red   / app->decor.div,
        bg_green  = (double) app->decor.bg_green / app->decor.div,
        bg_blue   = (double) app->decor.bg_blue  / app->decor.div,
        bg_alpha  = (double) app->decor.bg_alpha / app->decor.div;
    
    double fg_red = (double) app->decor.fg_red   / app->decor.div,
        fg_green  = (double) app->decor.fg_green / app->decor.div,
        fg_blue   = (double) app->decor.fg_blue  / app->decor.div,
        fg_alpha  = (double) app->decor.fg_alpha / app->decor.div;
    
    const GdkRGBA bg_widg = {bg_red, bg_green, bg_blue, bg_alpha};
    const GdkRGBA fg_widg = {fg_red, fg_green, fg_blue, fg_alpha};
    
    // Make window invisible
    gtk_widget_override_background_color(app->win, GTK_STATE_FLAG_NORMAL, &bg_win);
    gtk_widget_override_color(           app->win, GTK_STATE_FLAG_NORMAL, &fg_win);
    
    // Set widget color and transparency
    gtk_widget_override_background_color(app->widg, GTK_STATE_FLAG_NORMAL, &bg_widg);
    gtk_widget_override_color(           app->widg, GTK_STATE_FLAG_NORMAL, &fg_widg);
}



// Enable widget transparency
void enable_transparency(GtkWidget *widg) {
    
    // To check if the display supports alpha channels, get the visual 
    GdkScreen *screen = gtk_widget_get_screen(widg);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    
    // Set widget visual
    gtk_widget_set_visual(widg, visual);
}



// Setup the widget
void setup_widget(char *file, struct glmapp *app, char *event, void (*func)(GtkWidget *widg)) {
    
    // Define variables in preferences file
    set_pref_pos(  file, &app->pos);
    set_pref_txt(  file, &app->txt);
    set_pref_decor(file, &app->decor);
    
    // Setup the widget
    set_widget_pos(app);
    set_widget_color(app);
    enable_transparency(app->win);
    
    // Add widget to the application window
    gtk_container_add(GTK_CONTAINER(app->win), app->widg);
    
    // Application signals    
    g_signal_connect(app->win, "destroy", G_CALLBACK(gtk_main_quit), NULL);    
    
    if ( event != NULL )
        g_signal_connect(G_OBJECT(app->widg), event, G_CALLBACK(func), NULL);
    
    // Display the login frame
    gtk_widget_show(app->widg);
    gtk_widget_show(app->win);
}
