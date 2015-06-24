// Header guard
#ifndef XSETUP_H
#define XSETUP_H

// Includes
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <sys/types.h>
#include <dirent.h>

// Defines
#define X_CONFIG    "/etc/X11/elysia/src/login/config/Xsetup.config"

#define XCOMPMGR       "/usr/bin/xcompmgr"
#define XORG           "/usr/bin/Xorg"
#define XSERVER_AUTH   "/etc/X11/elysia/log/elysia.auth"
#define XSERVER_LOG    "/etc/X11/elysia/log/xserver.log"

// Public functions
void xsetup();

#endif
