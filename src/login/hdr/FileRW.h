#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef FILERW_H
#define FILERW_H

void file_write(char *file, char *phrase, char *fmt);
char * file_read(char *file);
char * get_open_display();
char ** command_line(char *cmd, int size);

#endif
