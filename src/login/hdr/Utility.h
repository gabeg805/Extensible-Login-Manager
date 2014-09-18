#include "../hdr/Config.h"

#include <gtk/gtk.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#ifndef UTILITY_H
#define UTILITY_H

void file_write(char *file, char *opt, const char *fmt, ...);
char * file_read(char *file);
char * get_open_display();
int get_open_tty();
char ** command_line(char *cmd, int size);
void cleanup_child(int signal);
struct glmgui * setup_gui_struct(GtkWidget *window, GtkWidget *widget, 
                                 const GdkRGBA bg_window, const GdkRGBA fg_window, 
                                 const GdkRGBA bg_widget, const GdkRGBA fg_widget, 
                                 int xpos, int ypos, int width, int height);
struct glmtext * setup_text_struct(GtkWidget *widget, char *font, int size, char *fmt);

#endif
