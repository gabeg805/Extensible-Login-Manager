// Includes
#include <gtk/gtk.h>

// Header guard
#ifndef UTILITY_H
#define UTILITY_H

// Public functions 
void cleanup_child(int signal);
int get_open_tty();
void file_write(char *file, char *opt, const char *fmt, ...);
char * file_read(char *file);
char * get_open_display();
char ** command_line(char *cmd, int size);
void enable_transparency(GtkWidget *widget);
void set_widget_color(GtkWidget *win, GtkWidget *widg, const GdkRGBA color[4]);
void setup_widget(GtkWidget *win, GtkWidget *widg, int pos[4], const GdkRGBA color[4]);

#endif
