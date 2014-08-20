#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef FILERW_H
#define FILERW_H

void file_write(char *file, char *opt, const char *fmt, ...);
char * file_read(char *file);
char * get_open_display();
int get_open_tty();
char ** command_line(char *cmd, int size);

#endif
