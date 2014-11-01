// Header guard
#ifndef UTILITY_H
#define UTILITY_H

// Includes
#include <gtk/gtk.h>

// Public functions 
void cleanup_child(int signal);
int count_char(char *str, char val);
int get_open_tty();
int get_open_display();
void file_write(char *file, char *opt, const char *fmt, ...);
void file_read(char *file, int ln, int sz, char *var);
void command_line(char *cmd, size_t sz, size_t sza, char *var);
void enable_transparency(GtkWidget *widget);
void set_widget_color(GtkWidget *win, GtkWidget *widg, const GdkRGBA color[4]);
void setup_widget(GtkWidget *win, GtkWidget *widg, int pos[4], const GdkRGBA color[4]);

#endif
