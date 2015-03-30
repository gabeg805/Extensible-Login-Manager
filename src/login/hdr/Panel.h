// Header guard
#ifndef PANEL_H
#define PANEL_H

// Includes
#include "../hdr/glm.h"
#include "../hdr/Utility.h"
#include <gtk/gtk.h>
#include <stdlib.h>

// Defines
#define PANEL_SHUT_CONFIG    "/etc/X11/glm/src/login/config/Panel-Shutdown.config"
#define PANEL_REF_CONFIG     "/etc/X11/glm/src/login/config/Panel-Refresh.config"
#define PANEL_REB_CONFIG     "/etc/X11/glm/src/login/config/Panel-Reboot.config"
#define PANEL_DIA_CONFIG     "/etc/X11/glm/src/login/config/Panel-Dialog.config"

#define POWEROFF          "/usr/bin/poweroff"
#define REBOOT            "/usr/bin/reboot"
#define SYSTEMCTL         "/usr/bin/systemctl"

// Public functions
void display_panel();

#endif
