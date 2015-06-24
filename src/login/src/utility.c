/* *****************************************************************************
 * 
 * Name:    utility.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Commonly used functions.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "utility.h"
#include "elytypes.h"
#include "elysia.h"
#include "benchmark.h"
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <gtk/gtk.h>
/* #include <sys/wait.h> */
/* #include <unistd.h> */



/* *************************
 * ***** PROGRAM USAGE *****
 * *************************
 */

void usage(char *prog)
{
    printf("Usage: %s [option]\n", prog);
    printf("\n");
    printf("Options:\n");
    printf("\t-h, --help       Print program usage.\n");
    printf("\t-v, --verbose    Print program information verbosely.\n");
    printf("\t-p, --preview    Run the login manager in Preview Mode, does not paint a new X window.\n");
    printf("\t-t, --time       Run benchmark time tests.\n");
}



// ///////////////////////////////
// ///// PARSE INPUT OPTIONS /////
// ///////////////////////////////

// Parse the command line arguments
void parse_argv(int argc, char **argv)
{
    const struct option long_options[] = {
        {"help",    optional_argument, 0, 'h'},
        {"verbose", optional_argument, 0, 'v'},
        {"preview", optional_argument, 0, 'p'},
        {"time",    optional_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    int opt;
    while ( (opt=getopt_long(argc, argv, "hvpt", long_options, 0)) != -1 ) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        case 'v':
            VERBOSE = true;
            break;
        case 'p':
            PREVIEW = true;
            break;
        case 't':
            BENCHTIME = true;
            break;
        default:
            break;
        }
    }
}



/* ************************************
 * ***** CLEANUP ZOMBIE PROCESSES *****
 * ************************************
 */

void cleanup_child(int signal)
{
    wait(NULL);
}



/* *****************************************
 * ***** GET SUBTRING WITHIN DELIMETER *****
 * *****************************************
 */

// Return the substring between the given separator
void get_substring(char *copy, char *str, char sep, int num)
{
    size_t i     = 0;
    size_t j     = 0;
    size_t count = 1;

    while ( str[i] != '\0' ) {
        if ( str[i] == sep )
            ++count;
        else if ( count == num ) {
            copy[j] = str[i];
            ++j;
        }

        if ( count > num ) 
            break;

        ++i;
    }
    copy[j] = 0;
}



/* *************************
 * ***** WRITE TO FILE *****
 * *************************
 */

/* Write to the Elysia log file */
void file_log(const char *fmt, ...)
{
    char *file = ELYSIA_LOG;
    char *opt = "a+";

    FILE *handle = fopen(file, opt);
    va_list args;

    va_start(args, fmt);
    vfprintf(handle, fmt, args);
    va_end(args);

    fclose(handle);
}



/* Overwrite configuration file line that matches the given key */
void file_line_overwrite(char *file, char *key, char *val)
{
    /* Prepare new strings for the file overwrite procedure */
    char *ext  = ".bak";
    size_t n   = strlen(file) + strlen(ext) + 1;
    size_t len = strlen(key)  + strlen(val) + 3;
    char newfile[n];
    char replacement[len];
    snprintf(newfile,     n,   "%s%s",   file, ext);
    snprintf(replacement, len, "%s: %s", key,  val);

    /* Edit the line matching the key */
    FILE *handle    = fopen(file, "r+");
    FILE *newhandle = fopen(newfile, "w");
    bool write      = false;
    char line[MAX_STR_LEN];
    char copy[MAX_STR_LEN];
    while ( fgets(line, sizeof(line), handle) != 0 ) {
        get_substring(copy, line, ':', 1);

        if ( (strcmp(key, copy) == 0) ) {
            fprintf(newhandle, "%s\n", replacement);
            write = true;
        } 
        else 
            fprintf(newhandle, "%s", line);
    }
    
    /* Write in default entry if key was not found */
    if ( !write )
        fprintf(handle, "%s: Not Found\n", key);
    

    fclose(handle);
    fclose(newhandle);
    remove(file);
    rename(newfile, file);
}



// ///////////////////////////////////////
// ///// CHECK IF PROGRAM IS RUNNING /////
// ///////////////////////////////////////

// Check if the specified program is running
bool is_running(char *prog)
{
    
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
void get_cmd_output(char *arr, int size, char *cmd)
{
    
    // Process attributes
    FILE *handle  = popen(cmd, "r");
    char line[MAX_STR_LEN];
    memset(arr, 0, size);
    
    // Initialize string lengths
    int i = 0,
        len = 0,
        loc = 0;
    
    // Add process output to array
    while ( fgets(line, MAX_STR_LEN, handle) != 0 ) {
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
char * read_config_char(char *file, char *key, int n)
{
    
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
                        assert(ret);
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
int read_config_int(char *file, char *key)
{
    char *val = read_config_char(file, key, MAX_NUM_LEN);
    
    if ( val == NULL )
        return -1;
    
    return atoi(val);
}



// Read a command from the config file and replace the '@' instances with the 
//   specified replacements
void read_config_cmd_rep(char *arr, char *file, char *rep1, char *rep2, char *rep3)
{
    
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
void exec_config_cmd(char *file, int n)
{
    
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
void set_config_pos(char *file, struct elypos *pos)
{
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
void set_config_txt(char *file, struct elytxt *txt)
{
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
void set_config_decor(char *file, struct elydecor *decor)
{
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
void set_widget_pos(struct elyapp *app)
{
    gtk_window_move(GTK_WINDOW(app->win), app->pos.x, app->pos.y);
    gtk_window_set_default_size(GTK_WINDOW(app->win), app->pos.width, app->pos.height);
}



// Set the color of a widget 
void set_widget_color(struct elyapp *app)
{
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
void enable_transparency(GtkWidget *widg)
{
    // To check if the display supports alpha channels, get the visual 
    GdkScreen *screen = gtk_widget_get_screen(widg);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    
    // Set widget visual
    gtk_widget_set_visual(widg, visual);
}



// Setup the widget
void setup_app(char *file, 
               struct elyapp *app, 
               char *event, 
               void (*func)(GtkWidget *widg))
{
    
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
