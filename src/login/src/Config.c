// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     Config.c
// 
// 
// SYNTAX: 
// 
//     Include the following header:
// 
//         #include "../hdr/Config.h"
// 
// 
// PURPOSE:
// 
//     Set global variables, to be used as a makeshift config file.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     N/A
// 
// 
// FILE STRUCTURE:
// 
//     N/A
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 18 2014 <> created
// 
//     gabeg Aug 20 2014 <> Made USERNAME, PASSWORD, SESSION, and INTERFACE global 
//                          variables.
// 
//     gabeg Sep 16 2014 <> Removed unneeded libraries
// 
// **********************************************************************************



// Includes
#include "../hdr/Config.h"

#include <gtk/gtk.h>


// Important
int PREVIEW = 0;
int TTY_N = 4;
int INTERFACE = 0;
char *DISPLAY = ":0";
char *USERNAME = "User";
char *PASSWORD = "Password";
char *SESSION = "xterm";



// Position
int WINDOWMANAGER_XPOS    = 770;
int WINDOWMANAGER_YPOS    = 315;



// Size
int WINDOWMANAGER_WIDTH    = 30;
int WINDOWMANAGER_HEIGHT    = 30;



// Colors
const GdkRGBA BG_WINDOWMANAGER = {0, 0, 0, 0};
const GdkRGBA FG_WINDOWMANAGER = {1, 1, 1, 1};



// Fonts


// Utility Files
char *SERVICE        = "glm";
char *WTMP           = "/var/log/wtmp";
char *UTMP_ADD       = "/run/utmp";
char *UTMP_DEL       = "/var/run/utmp";
char *XSERVER_AUTH   = "/etc/X11/glm/log/glm.auth";

// Log Files
char *GLM_LOG             = "/etc/X11/glm/log/glm.log";
char *XSERVER_LOG         = "/etc/X11/glm/log/xserver.log";
char *TTY_LOG_DIR         = "/etc/X11/glm/log/tty/";
char *USERNAME_LOG        = "/etc/X11/glm/log/user.log";
char *SESSION_LOG         = "/etc/X11/glm/log/session.log";

// Image Files
char *WALLPAPER            = "/etc/X11/glm/img/wallpapers/night-sky.jpg";
char *USERNAME_IMG         = "/etc/X11/glm/img/interface/user.png";
char *WINDOWMANAGER_IMG    = "/etc/X11/glm/img/interface/settings.png";



// Commands
char *DATE       = "/usr/bin/date";
char *XORG       = "/usr/bin/Xorg";
char *XCOMPMGR   = "/usr/bin/xcompmgr";
char *HSETROOT   = "/usr/bin/hsetroot";
char *XSETROOT   = "/usr/bin/xsetroot";
char *AWK        = "/usr/bin/awk";
char *GREP       = "/usr/bin/grep";
char *TAIL       = "/usr/bin/tail";
char *LOGINCTL   = "/usr/bin/loginctl";
char *SYSTEMCTL  = "/usr/bin/systemctl";
char *POWEROFF   = "/usr/bin/poweroff";
char *REBOOT     = "/usr/bin/urxvt";
char *SESSREG    = "/usr/bin/sessreg";
char *PQIV       = "/usr/bin/pqiv";
char *XINITRC    = "/etc/X11/glm/src/x/xinitrc";

char *WM_SES_CMD               = "ls -1 /usr/share/xsessions/ | sed 's/.desktop//'";
