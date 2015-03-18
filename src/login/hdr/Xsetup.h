// Header guard
#ifndef XSETUP_H
#define XSETUP_H

// Includes
#include "../hdr/glm.h"
#include "../hdr/Utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Defines
#define DATE           "/usr/bin/date"
#define HSETROOT       "/usr/bin/hsetroot"
#define TAIL           "/usr/bin/tail"
#define WALLPAPER      "/etc/X11/glm/img/wallpapers/night-sky.jpg"
#define XCOMPMGR       "/usr/bin/xcompmgr"
#define XORG           "/usr/bin/Xorg"
#define XSERVER_AUTH   "/etc/X11/glm/log/glm.auth"
#define XSERVER_LOG    "/etc/X11/glm/log/xserver.log"
#define XSETROOT       "/usr/bin/xsetroot"

// Public functions
void xsetup();

#endif
