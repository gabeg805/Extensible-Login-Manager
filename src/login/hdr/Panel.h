// Header guard
#ifndef PANEL_H
#define PANEL_H

// Includes
#include "../hdr/glm.h"
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <stdlib.h>

// Defines
#define PANEL_SHUT_PREF    "/etc/X11/glm/src/login/pref/Panel-Shutdown.pref"
#define PANEL_REF_PREF     "/etc/X11/glm/src/login/pref/Panel-Refresh.pref"
#define PANEL_REB_PREF     "/etc/X11/glm/src/login/pref/Panel-Reboot.pref"
#define PANEL_DIA_PREF     "/etc/X11/glm/src/login/pref/Panel-Dialog.pref"

#define POWEROFF          "/usr/bin/poweroff"
#define REBOOT            "/usr/bin/reboot"
#define SYSTEMCTL         "/usr/bin/systemctl"

// Public functions
void display_panel();

#endif
