// Header guard
#ifndef UTILITY_H
#define UTILITY_H

// Includes
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
#define READ_INT_LEN     20
#define READ_CHAR_LEN    50
#define READ_PATH_LEN    100

// Definitions
struct glmpos { // Widget position and size
    int x;
    int y;
    int width;
    int height;
};

struct glmtxt { // Text attributes
    int size;
    int maxchars;
    
    char *text;
    char *font;
    char *fmt;
    char invis;
    
    int red;
    int green;
    int blue;
};

struct glmdecor { // Widget decoration
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

struct glmapp { // Widget application
    GtkWidget *win;
    GtkWidget *widg;
    struct glmpos pos;
    struct glmtxt txt;
    struct glmdecor decor;
};


// Public functions 
void cleanup_child(int signal);
int count_char(char *str, char val);
void file_write(char *file, char *opt, const char *fmt, ...);
char * file_read(char *file, int ln, int sz);
char * command_line(char *cmd, size_t sz, size_t sza);

void read_pref_char(char *ret, int n, int m, char *file, char *key);
void read_pref_int(int *ret, int n, char *file, char *key);

void set_pref_pos(char *file, struct glmpos *pos);
void set_pref_txt(char *file, struct glmtxt *txt);
void set_pref_decor(char *file, struct glmdecor *decor);

void set_widget_pos(struct glmapp *app);
void set_widget_color(struct glmapp *app);
void enable_transparency(GtkWidget *widget);
void setup_widget(char *file, struct glmapp *app, char *event, void (*func)(GtkWidget *widg));

#endif
