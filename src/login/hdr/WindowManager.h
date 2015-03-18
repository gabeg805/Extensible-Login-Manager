// Header guard
#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

// Includes
#include "../hdr/glm.h"
#include "../hdr/Utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

// Defines
#define WM_PREF    "/etc/X11/glm/src/login/pref/WindowManager.pref"

#define WM_SES_CMD    "ls -1 /usr/share/xsessions/ | sed 's/.desktop//'"
#define SESSION_LOG   "/etc/X11/glm/log/session.log"

// Public functions
void display_window_manager();

#endif 
