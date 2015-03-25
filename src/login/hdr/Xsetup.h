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
#define X_PREF    "/etc/X11/glm/src/login/pref/Xsetup.pref"

#define DATE           "/usr/bin/date"
#define TAIL           "/usr/bin/tail"
#define XCOMPMGR       "/usr/bin/xcompmgr"
#define XORG           "/usr/bin/Xorg"
#define XSERVER_AUTH   "/etc/X11/glm/log/glm.auth"
#define XSERVER_LOG    "/etc/X11/glm/log/xserver.log"

// Public functions
void xsetup();

#endif
