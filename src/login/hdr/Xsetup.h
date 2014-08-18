#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef XSETUP_H
#define XSETUP_H

void start_xserver();
void start_compman();
void xsetup(int preview);

#endif
