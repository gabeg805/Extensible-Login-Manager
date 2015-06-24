// *****************************************************************************
// 
// Name:    utility.h
// Author:  Gabriel Gonzalez
// Email:   gabeg@bu.edu
// License: The MIT License (MIT)
// 
// Syntax: #include "utility.h"
// 
// Description: Contains the variables and types used by all Elysia source 
//              files.
//              
// Notes: None.
// 
// *****************************************************************************



// Header guard
#ifndef UTILITY_H
#define UTILITY_H

// Includes
#include "elytypes.h"

#include <stdbool.h>
#include <gtk/gtk.h>

// Defines
#define MAX_CMD_LEN    128
#define MAX_LOC_LEN    64
#define MAX_STR_LEN    32
#define MAX_NUM_LEN    16

// Public functions 
void usage(char *prog);
void parse_argv(int argc, char **argv);
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
