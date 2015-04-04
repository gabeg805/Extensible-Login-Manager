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
//     get_substring       - Return the substring in between the specified delimeter.
// 
//     file_log            - Write to the GLM log file.
//     file_line_overwrite - Overwrite the specified line (given by the key) in the 
//                           configuration file.
// 
//     is_running          - Check if program is running.
// 
//     get_cmd_output      - Get output from the specified linux command.
// 
//     read_config_char    - Read the config file and output a string.
//     read_config_int     - Read the config file and output an int.
//     read_config_cmd_rep - Read the config file and output a command that has 
//                           replacement chars ('@'), and replace those chars with
//                           the specified replacements.
//     exec_config_cmd     - Execute the command in the config file.
// 
// 
//     set_config_pos      - Set position values defined in the config file.
//     set_config_txt      - Set text values defined in the config file.
//     set_config_decor    - Set decoration values defined in the config file.
// 
//     set_widget_pos      - Set the size and position of the widget.
//     set_widget_color    - Set the color and opacity of the widget.
//     enable_transparency - Enable widget transparency.
//     setup_app           - Setup the widget application.
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Parse Input Options
//     * Remove Zombie Processes
//     * Get Subtring Within Delimeter
//     * Write to File
//     * Check If Program Is Running
//     * Get Linux Command Output
//     * Read Config File
//     * Set Config Values
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
//     gabeg Apr 04 2015 <> Added a function "file_line_overwrite" to overwrite the
//                          line that matches the given key, in the given file. This
//                          will make "file_read" obselete as I put the items I was
//                          reading into config files and then use the "read_config" 
//                          functions to find the items I was looking for. 
//                          
//                          Modified the "MAX_LEN" variables to include one for 
//                          commands, and used the "MAX_CMD_LEN" in the 
//                          "exec_config_cmd" function.
//                          
//                          Removed the "file_read" function, it's now obsolete.
// 
//                          Removed the "file_write" function, it's now obsolete.
// 
//                          Added the "get_cmd_output" function to replace 
//                          "command_line".
// 
//                          Removed the "command_line" function, it's now obsolete.
// 
//                          Added the "read_config_cmd_rep" function which reads a  
//                          command from the config file and replaces instances of
//                          '@' (up to 3), with whatever the user specifies.
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



// ///////////////////////////////////////
// ///// INITIALIZE GLOBAL VARIABLES /////
// /////////////////////...../////////////

// Initialize global variables
/* void init_globals() { */
/*     SERVICE  = "glm"; */
/*     USERNAME = "User"; */
/*     PASSWORD = "Password"; */
/*     SESSION  = "xterm"; */
/*     GLM_LOG  = "/etc/X11/glm/log/glm.log"; */
/*     TTYN      = 1; */
/*     INTERFACE = false; */
/*     PREVIEW   = false; */
/*     VERBOSE   = false; */
/*     BENCHTIME = false; */
/* } */



// ///////////////////////////////////
// ///// REMOVE ZOMBIE PROCESSES /////
// ///////////////////////////////////

// Clean up child zombie process
void cleanup_child(int signal) {
    wait(NULL);
}



// /////////////////////////////////////////
// ///// GET SUBTRING WITHIN DELIMETER /////
// /////////////////////////////////////////

// Return the substring between the given separator
void get_substring(char *copy, char *str, char sep, int num) {
    
    // Loop variables
    int i     = 0,
        j     = 0,
        count = 1;
    
    // Loop through each variable in array
    while ( str[i] != '\0' ) {
        
        // Increae separator count
        if ( str[i] == sep )
            ++count;
        else if ( count == num ) {
            // Copy current char 
            copy[j] = str[i];
            ++j;
        }
        
        // Done checking for separator
        if ( count > num ) 
            break;
        
        ++i;
    }
    
    // Add null terminator
    copy[j] = '\0';
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



// Overwrite configuration file line that matches the given key
void file_line_overwrite(char *file, char *key, char *val) {
    
    // File attributes    
    FILE *handle = fopen(file, "r+");
    char line[MAX_STR_LEN];
    char copy[MAX_STR_LEN];
    bool write = false;
    
    // New file attributes
    char *ext = ".bak";
    int n = strlen(file) + strlen(ext) + 1;
    char newfile[n];
    snprintf(newfile, n, "%s%s", file, ext);
    
    FILE *new_handle = fopen(newfile, "w");
    
    // Create the new line
    int len_key = strlen(key),
        len_val = strlen(val),
        len     = len_key + len_val + 3;
    char newline[len];
    snprintf(newline, len, "%s: %s", key, val);
    
    // Edit the line matching the key, in the file
    while ( fgets(line, sizeof(line), handle) != NULL ) {
        
        // Find the key
        get_substring(copy, line, ':', 1);
        
        // Rewrite a new file with same contents but the line is changed
        if ( (strcmp(key, copy) == 0) ) {
            fprintf(new_handle, "%s\n", newline);
            write = true;
        } else 
            fprintf(new_handle, "%s", line);
    }
    
    // Write in default entry if key was not found
    if ( !write )
        fprintf(handle, "%s: Not Found\n", key);
    
    // Close the files
    fclose(handle);
    fclose(new_handle);
    
    // Remove old file and rename the new file 
    remove(file);
    rename(newfile, file);
}



// ///////////////////////////////////////
// ///// CHECK IF PROGRAM IS RUNNING /////
// ///////////////////////////////////////

// Check if the specified program is running
bool is_running(char *prog) {
    
    // File and directory reading variables
    DIR  *dir_handle = opendir("/proc");
    FILE *file_handle;
    struct dirent *dir_entry;
    char *endptr;
    char buf[512];
    
    // Loop through each file name in the directory
    while ( (dir_entry=readdir(dir_handle)) != NULL ) {
        
        // Convert filename to pid value
        long pid = strtol(dir_entry->d_name, &endptr, 10);
        
        if ( *endptr != '\0' ) 
            continue;
        
        // Open up the cmdline file
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", pid);
        file_handle = fopen(buf, "r");
        
        // Read the cmdline file
        if ( fgets(buf, sizeof(buf), file_handle) != NULL ) {
            int i   = 0, 
                j   = 0,
                sz  = strlen(buf),
                len = strlen(prog);
            char copy[sz+1];
            bool match = false,
                clear  = false;
            
            // Loop through each char of the line in the file
            while ( buf[i] != '\0' ) {
                
                // Stop searching if too late to match
                if ( (j+(sz-i)) < len )
                    break;
                
                // Matched char found (keeps adding to match until space char found)
                if ( (buf[i] != ' ') && ((buf[i] == prog[j]) || match) ) {
                    
                    // Clear memory
                    if ( !clear && (j == 0) ) {
                        memset(copy, 0, sz+1);
                        clear = true;
                    }
                    
                    // Continue matching chars (if possible)
                    if ( j == (len-1) )
                        match = true;
                    
                    copy[j] = buf[i];
                    ++j;
                } else {
                    // Match not found, reset everything
                    if ( j >= len )
                        break;
                    if ( j != 0 )
                        memset(copy, 0, j);
                    
                    j = 0;
                }
                ++i;
            }
            
            // Check for match
            if ( strcmp(prog, copy) == 0 ) 
                return true;
        }
        
        // Close the file
        fclose(file_handle);
    }
    
    // Close the directory
    closedir(dir_handle);
        
    return false;
}



// ////////////////////////////////////
// ///// GET LINUX COMMAND OUTPUT ///// 
// ////////////////////////////////////


// Store command output as a string inside variable
void get_cmd_output(char *arr, int size, char *cmd) { 
    
    // Process attributes
    FILE *handle  = popen(cmd, "r");
    char line[MAX_STR_LEN];
    memset(arr, 0, size);
    
    // Initialize string lengths
    int i = 0,
        len = 0,
        loc = 0;
    
    // Add process output to array
    while (fgets(line, MAX_STR_LEN, handle) != NULL ) {
        len = strlen(arr) + strlen(line);
        
        if ( size > len ) {
            i = 0;
            
            while ( line[i] != '\0' ) {
                arr[loc] = line[i];
                ++i;
                ++loc;
            }
        }
    }
    
    // Close process
    pclose(handle);
}



// ////////////////////////////
// ///// READ CONFIG FILE /////
// ////////////////////////////

// Read the config file and output a string
char * read_config_char(char *file, char *key, int n) {
    
    // File variables
    FILE *handle = fopen(file, "r");
    char *ret = NULL;
    char line[n];
    
    // Substring variables
    char sep = ':';
    int count    = 0,
        len_line = 0,
        num      = 1;
    
    // Array indicies
    int i = 0,
        j = 0;
    
    // Loop through line by line the contents of the config file
    while ( fgets(line, sizeof(line), handle) != NULL ) {
        
        // Reset to 0
        i     = 0;
        j     = 0;
        count = 0;
        
        // Find the key substring in the file line
        char sub[n];
        get_substring(sub, line, sep, num);
        
        if ( strcmp(key, sub) != 0 )
            continue;
        
        // Loop through each char of the line if key was found
        while ( line[i] != 0 ) {
            
            // Compare attribute name to given key
            if ( count < num ) {
                if ( line[i] == sep )
                    ++count;
            } else {
                
                // Attribute found, ignore leading space and trailing newline
                bool is_start  = (j == 0),
                    is_space   = ((line[i] == ' ') || (line[i] == '\t') || (line[i] == '\n')),
                    is_newline = (line[i] == '\n'),
                    is_end     = (i == len_line-2);
                
                if ( ( is_start && is_space ) || ( is_end && is_newline ) )
                    ;
                else {
                    
                    // Allocate and clear memory for return variable
                    if ( is_start ) {
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
        break;
    }
    
    // Close the file
    fclose(handle);
    
    return ret;
}



// Read config file and output an int
int read_config_int(char *file, char *key) {
    char *val = read_config_char(file, key, MAX_NUM_LEN);
    
    if ( val == NULL )
        return -1;
    
    return atoi(val);
}



// Read a command from the config file and replace the '@' instances with the 
//   specified replacements
void read_config_cmd_rep(char *arr, char *file, char *rep1, char *rep2, char *rep3) {
    
    // Clear memory of holder array and define command string 
    memset(arr, 0, MAX_CMD_LEN);
    char *cmd = read_config_char(file, "cmd", MAX_CMD_LEN);
    
    // Counters for arrays
    int i     = 0,
        j     = 0,
        count = 1;
    
    // Copy command string and replace instances of '@' with the given parameters
    while ( cmd[i] != '\0' ) {
        
        // Copy over command char by char
        if ( cmd[i] != '@' ) {
            arr[j] = cmd[i];
            ++j;
        } else {
            
            // Replacement char found, copy over replacement strings
            switch ( count ) {
            case 1:
                if ( rep1 == NULL ) break;
                
                strcat(arr, rep1);
                j = j + strlen(rep1);
                break;
            case 2:
                if ( rep2 == NULL ) break;
                
                strcat(arr, rep2);
                j = j + strlen(rep2);
                break;
            case 3:
                if ( rep3 == NULL ) break;
                
                strcat(arr, rep3);
                j = j + strlen(rep3);
                break;
            default:
                break;
            }
            
            ++count;
        }
        
        ++i;
    }
}



// Execute a command found in the config file
void exec_config_cmd(char *file, int n) {
    
    // Get the command string from the config file
    char key[6];
    snprintf(key, sizeof(key), "cmd%d", n);
    char *cmd = read_config_char(file, key, MAX_CMD_LEN);
    
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
    
    // Set default values
    if ( pos->x < 0 )      pos->x = 0;
    if ( pos->y < 0 )      pos->y = 0;
    if ( pos->width < 0 )  pos->width = 0;
    if ( pos->height < 0 ) pos->height = 0;
}



// Set config text values
void set_config_txt(char *file, struct glmtxt *txt) {
    txt->size     = read_config_int(file, "size");
    txt->maxchars = read_config_int(file, "maxchars");
    txt->refresh  = read_config_int(file, "refresh-sec");
    
    txt->text  = read_config_char(file, "text",  MAX_STR_LEN);
    txt->font  = read_config_char(file, "font",  MAX_STR_LEN);
    txt->fmt   = read_config_char(file, "fmt",   MAX_STR_LEN);
    txt->invis = read_config_char(file, "invis", MAX_STR_LEN);
    
    txt->red   = read_config_int(file, "txt-red");
    txt->green = read_config_int(file, "txt-green");
    txt->blue  = read_config_int(file, "txt-blue");
    
    // Set default values
    if ( txt->invis == NULL )
        txt->invis = " ";
}



// Set config decoration values
void set_config_decor(char *file, struct glmdecor *decor) {
    decor->img_file = read_config_char(file, "img-file", MAX_LOC_LEN);
    
    decor->bg_red   = read_config_int(file, "bg-red");
    decor->bg_green = read_config_int(file, "bg-green");
    decor->bg_blue  = read_config_int(file, "bg-blue");
    decor->bg_alpha = read_config_int(file, "bg-alpha");
    
    decor->fg_red   = read_config_int(file, "fg-red");
    decor->fg_green = read_config_int(file, "fg-green");
    decor->fg_blue  = read_config_int(file, "fg-blue");
    decor->fg_alpha = read_config_int(file, "fg-alpha");
    
    decor->div = read_config_int(file, "div");
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
    
    // No decoration defined
    if ( app->decor.div < 0 )
        return;
    
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
void setup_app(char *file, 
               struct glmapp *app, 
               char *event, 
               void (*func)(GtkWidget *widg)) {
    
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
