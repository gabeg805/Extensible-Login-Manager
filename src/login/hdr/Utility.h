// Header guard
#ifndef UTILITY_H
#define UTILITY_H

// Includes
#include "../hdr/Benchmark.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <gtk/gtk.h>

// Defines
#define MAX_CMD_LEN    128
#define MAX_LOC_LEN    64
#define MAX_STR_LEN    32
#define MAX_NUM_LEN    16

// Global variables
extern char *SERVICE;
extern char *USERNAME;
extern char *PASSWORD;
extern char *SESSION;
extern char *ELYSIA_LOG;
extern int  TTYN;
extern bool INTERFACE;
extern bool PREVIEW;
extern bool VERBOSE;
extern bool BENCHTIME;

// Definitions
struct elypos { // Widget position and size
    int x;
    int y;
    int width;
    int height;
};

struct elytxt { // Text attributes
    int size;
    int maxchars;
    int refresh;
    
    char *text;
    char *font;
    char *fmt;
    char *invis;
    
    int red;
    int green;
    int blue;
};

struct elydecor { // Widget decoration
    char *img_file;
    
    int bg_red;
    int bg_green;
    int bg_blue;
    int bg_alpha;
    
    int fg_red;
    int fg_green;
    int fg_blue;
    int fg_alpha;
    
    int div;
};

struct elyapp { // Widget application
    GtkWidget *win;
    GtkWidget *widg;
    struct elypos pos;
    struct elytxt txt;
    struct elydecor decor;
};


// Public functions 
void cli_parse(int argc, char **argv);
/* void init_globals(); */
void cleanup_child(int signal);

void get_substring(char *copy, char *str, char sep, int num);
void file_log(const char *fmt, ...);
void file_line_overwrite(char *file, char *key, char *val);

bool is_running(char *prog);
void get_cmd_output(char *arr, int size, char *cmd);

char * read_config_char(char *file, char *key, int n);
int read_config_int(char *file, char *key);
void read_config_cmd_rep(char *arr, char *file, char *rep1, char *rep2, char *rep3);
void exec_config_cmd(char *file, int n);

void set_config_pos(char *file, struct elypos *pos);
void set_config_txt(char *file, struct elytxt *txt);
void set_config_decor(char *file, struct elydecor *decor);

void set_widget_pos(struct elyapp *app);
void set_widget_color(struct elyapp *app);
void enable_transparency(GtkWidget *widget);
void setup_app(char *file, 
               struct elyapp *app, 
               char *event, 
               void (*func)(GtkWidget *widg));

#endif
