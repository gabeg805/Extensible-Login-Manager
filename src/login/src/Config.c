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
//     Without a main function, include the following header:
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
// **********************************************************************************



// Includes
#include "../hdr/Config.h"


// Commands
char *DATE       = "/usr/bin/date";
char *XORG       = "/usr/bin/Xorg";
char *XCOMPMGR   = "/usr/bin/xcompmgr";
char *AWK        = "/usr/bin/awk";
char *GREP       = "/usr/bin/grep";
char *TAIL       = "/usr/bin/tail";
char *LOGINCTL   = "/usr/bin/loginctl";
char *SYSTEMCTL  = "/usr/bin/systemctl";
char *POWEROFF   = "/usr/bin/poweroff";
char *REBOOT     = "/usr/bin/reboot";
char *SESSREG    = "/usr/bin/sessreg";
char *PQIV       = "/usr/bin/pqiv";
char *XINITRC    = "/etc/X11/glm/src/x/xinitrc";

char *WM_SES_CMD               = "ls -1 /usr/share/xsessions/ | sed 's/.desktop//'";



// Position
int FRAME_XPOS            = 550;
int TEXTIMAGE_XPOS        = 570;
int USERNAME_XPOS         = 665;
int PASSWORD_XPOS         = 570;
int WINDOWMANAGER_XPOS    = 770;
int CLOCK_DATE_XPOS       = 50;
int CLOCK_TIME_XPOS       = 86;
int PANEL_SHUTDOWN_XPOS   = 1366-32*1;
int PANEL_REBOOT_XPOS     = 1366-32*2;
int PANEL_REFRESH_XPOS    = 1366-32*3;
int PANEL_DIALOG_XPOS     = 505;

int FRAME_YPOS            = 300;
int TEXTIMAGE_YPOS        = 330;
int USERNAME_YPOS         = 197;
int PASSWORD_YPOS         = 350;
int WINDOWMANAGER_YPOS    = 315;
int CLOCK_DATE_YPOS       = 650;
int CLOCK_TIME_YPOS       = 575;
int PANEL_SHUTDOWN_YPOS   = 768-32;
int PANEL_REBOOT_YPOS     = 768-32;
int PANEL_REFRESH_YPOS    = 768-32;
int PANEL_DIALOG_YPOS     = 310;



// Size
int FRAME_WIDTH            = 265;
int USERNAME_WIDTH         = 150;
int PASSWORD_WIDTH         = 230;
int WINDOWMANAGER_WIDTH    = 30;
int CLOCK_DATE_HEIGHT      = 50;
int CLOCK_TIME_HEIGHT      = 75;
int PANEL_SHUTDOWN_WIDTH   = 32;
int PANEL_REBOOT_WIDTH     = 32;
int PANEL_REFRESH_WIDTH    = 32;

int FRAME_HEIGHT            = 100;
int USERNAME_HEIGHT         = 50;
int PASSWORD_HEIGHT         = 30;
int WINDOWMANAGER_HEIGHT    = 30;
int CLOCK_DATE_WIDTH        = 400;
int CLOCK_TIME_WIDTH        = 400;
int PANEL_SHUTDOWN_HEIGHT   = 32;
int PANEL_REBOOT_HEIGHT     = 32;
int PANEL_REFRESH_HEIGHT    = 32;



// Fonts
int TEXTIMAGE_FSIZE     = 12;
int USERNAME_FSIZE      = 23*1024;
int PASSWORD_FSIZE      = 10*1024;
int CLOCK_DATE_FSIZE    = 25*1024;
int CLOCK_TIME_FSIZE    = 45*1024;

char *TEXTIMAGE_FONT    = "DejaVu Sans";
char *USERNAME_FONT     = "DejaVu Sans";
char *PASSWORD_FONT     = "Inconsolata";
char *CLOCK_DATE_FONT   = "Inconsolata";
char *CLOCK_TIME_FONT   = "Inconsolata";



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
char *WINDOWMANAGER_LOG   = "/etc/X11/glm/log/session.log";


// Image Files
char *USERNAME_IMG         = "/etc/X11/glm/img/interface/user.png";
char *WINDOWMANAGER_IMG    = "/etc/X11/glm/img/interface/settings.png";
char *PANEL_SHUTDOWN_IMG   = "/etc/X11/glm/img/interface/shutdown.png";
char *PANEL_REBOOT_IMG     = "/etc/X11/glm/img/interface/reboot.png";
char *PANEL_REFRESH_IMG    = "/etc/X11/glm/img/interface/refresh.png";



// Misc
char *TEXTIMAGE_TEXT   = "Password:";


char *CLOCK_DATE_FMT   = "%A, %B %-d";
char *CLOCK_TIME_FMT   = "%-I:%M %p";
int CLOCK_UPDATE_SEC   = 30;

char PASSWORD_INVISCHAR   = '*';
int PASSWORD_MAXCHARS     = 30;

int TTY_N = 4;

char *DISPLAY = ":0";

int INTERFACE = 0;
