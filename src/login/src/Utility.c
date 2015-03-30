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
// OPTIONS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     cli_parse           - Parse the command line arguments.
// 
//     cleanup_child       - Remove zombie processes.
// 
//     count_char          - Count number of times a character occurs in a string.
// 
//     file_log            - Write to the GLM log file.
//     file_write          - Write to a file.
// 
//     file_read           - Read a file's contents.
// 
//     command_line        - Return output of a Linux command.
// 
//     read_config_char    - Read the config file and output a string.
//     read_config_int     - Read the config file and output an int.
// 
//     set_config_pos      - Set position values defined in the config file.
//     set_config_txt      - Set text values defined in the config file.
//     set_config_decor    - Set decoration values defined in the config file.
// 
//     set_widget_pos      - Set the size and position of the widget.
//     set_widget_color    - Set the color and opacity of the widget.
//     enable_transparency - Enable widget transparency.
//     setup_app           - Setup the application.
// 
// 
// FILE STRUCTURE:
// 
//     * Parse Input Options
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
//                          value pairs from a config file. In doing so I remove
//                          all the preprocessor define statements and just put all 
//                          the variables I need into glm structures. This hopefully 
//                          makes the code look cleaner.
// 
//     gabeg Mar 19 2015 <> Attempted to make code look cleaner by creating a 
//                          universal setup function for each application by 
//                          incorporating a struct 'glmapp' that holds everything an
//                          app will need.
// 
//     gabeg Mar 25 2015 <> Modified the "read_config_char" function so that it 
//                          allocates memory if it finds a match, that way the user 
//                          doesn't have to manually malloc. Also, the function takes
//                          in less arguments than before. Modified the 
//                          "read_config_int" function so that it utilizes the 
//                          "read_config_char" function, that way I don't repeat 
//                          essentially the same code, then I just convert the string
//                          it finds to an int. Added a function that reads and 
//                          executes commands from the config file, called 
//                          "exec_config_cmd". 
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/Utility.h"



// ///////////////////////////////
// ///// PARSE INPUT OPTIONS /////
// ///////////////////////////////

// Parse the command line arguments
void cli_parse(int argc, char **argv) {
    
    int i = 0;
    
    while ( i < argc ) {
        if ( (strcmp(argv[i], "-p") == 0) || (strcmp(argv[i], "--preview") == 0) )
            PREVIEW = true;
        
        if ( (strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--verbose") == 0) )
            VERBOSE = true;
        
        if ( (strcmp(argv[i], "-t") == 0) || (strcmp(argv[i], "--time") == 0) )
            BENCHTIME = true;
        
        ++i;
    }
}



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

// Log to the GLM log file
void file_log(const char *fmt, ...) {
    
    // Default values for logging
    char *file = GLM_LOG;
    char *opt = "a+";
    
    // Write to file
    FILE *handle = fopen(file, opt);
    va_list args;
    
    va_start(args, fmt);
    vfprintf(handle, fmt, args);
    va_end(args);
    
    fclose(handle);
}



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




// ////////////////////////////
// ///// READ CONFIG FILE /////
// ////////////////////////////

// Read the config file and output a string
char * read_config_char(char *file, char *key, int n) {
    
    // File variables
    FILE *handle = fopen(file, "r");
    char line[n];
    char *ret = NULL;
    
    // Length of various strings
    int count    = 0,
        len_key  = strlen(key),
        len_line = 0;
    
    // Array indicies
    int i = 0,
        j = 0;
    
    // Loop through line by line the contents of the config file
    while ( fgets(line, sizeof(line), handle) != NULL ) {
        
        // Reset to 0
        count = 0;
        i = 0; 
        j = 0;
        
        bool sep  = false,
            found = false;
        
        // Loop through each char of the line
        while ( line[i] != 0 ) {
            
            // Compare attribute name to given key
            if ( !sep || !found ) {
                
                if ( line[i] == ':' )
                    sep = true;
                
                if ( !found && (i < len_key) && (line[i] == key[i]) )
                    ++count;
                
                if ( count == len_key )
                    found = true;
                
            } else {
                
                // Attribute found, ignore leading space and trailing newline
                if ( 
                     ( (j == 0) && ((line[i] == ' ') || (line[i] == '\t') || (line[i] == '\n')) ) 
                     || 
                     ( (i == len_line-2) && (line[i] == '\n') )
                   )
                    ;
                else {
                    
                    // Allocate and clear memory for return variable
                    if ( j == 0 ) {
                        // Create your own strlen to ignore leading spaces
                        len_line = strlen(line) + 1; 
                        ret = malloc(len_line);
                        memset(ret, 0, len_line);
                    }
                    
                    // Store value of attribute
                    ret[j] = line[i];
                    ++j;
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
    
    return ret;
}



// Read config file and output an int
int read_config_int(char *file, char *key) {
    char *val = read_config_char(file, key, READ_INT_LEN);
    
    if ( val == NULL )
        return 0;
    
    return atoi(val);
}



// Execute a command found in the config file
void exec_config_cmd(char *file, int n) {
    
    // Get the command string from the config file
    char key[6];
    snprintf(key, sizeof(key), "cmd%d", n);
    char *cmd = read_config_char(file, key, READ_PATH_LEN);
    
    // Clean up zombie processes
    signal(SIGCHLD, cleanup_child);
    
    // Execute the command
    pid_t pid = fork();
    
    if ( pid == 0 ) {
        system(cmd);
        exit(0);
    }
    
    
}



// /////////////////////////////
// ///// SET CONFIG VALUES /////
// /////////////////////////////

// Set config position values
void set_config_pos(char *file, struct glmpos *pos) {
    pos->x      = read_config_int(file, "xpos");
    pos->y      = read_config_int(file, "ypos");
    pos->width  = read_config_int(file, "width");
    pos->height = read_config_int(file, "height");
}



// Set config text values
void set_config_txt(char *file, struct glmtxt *txt) {
    txt->size     = read_config_int(file, "size");
    txt->maxchars = read_config_int(file, "maxchars");
    
    txt->text  = read_config_char(file, "text",  READ_CHAR_LEN);
    txt->font  = read_config_char(file, "font",  READ_CHAR_LEN);
    txt->fmt   = read_config_char(file, "fmt",   READ_CHAR_LEN);
    txt->invis = read_config_char(file, "invis", READ_CHAR_LEN);
    
    txt->red   = read_config_int(file, "txt-red");
    txt->green = read_config_int(file, "txt-green");
    txt->blue  = read_config_int(file, "txt-blue");
}



// Set config decoration values
void set_config_decor(char *file, struct glmdecor *decor) {
    decor->img_file = read_config_char(file, "img-file", READ_PATH_LEN);
    
    decor->bg_red   = read_config_int(file, "bg-red");
    decor->bg_green = read_config_int(file, "bg-green");
    decor->bg_blue  = read_config_int(file, "bg-blue");
    decor->bg_alpha = read_config_int(file, "bg-alpha");
    
    decor->fg_red   = read_config_int(file, "fg-red");
    decor->fg_green = read_config_int(file, "fg-green");
    decor->fg_blue  = read_config_int(file, "fg-blue");
    decor->fg_alpha = read_config_int(file, "fg-alpha");
    
    decor->div = read_config_int(file, "div");
    
    // Set default value to 1
    if ( decor->div == 0 )
        decor->div = 1;
}



// /////////////////////////////
// ///// APPLICATION SETUP /////
// /////////////////////////////

// Set widget position and size
void set_widget_pos(struct glmapp *app) {
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
    
    double bmtime = benchmark_runtime(0);
    
    // Define variables in config file
    set_config_pos(file,   &app->pos);
    set_config_txt(file,   &app->txt);
    set_config_decor(file, &app->decor);
    
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
    
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}
