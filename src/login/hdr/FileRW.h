#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef FILERW_H
#define FILERW_H

void file_write(char *file, char *phrase, char *fmt);
char * file_read(char *file);

#endif
