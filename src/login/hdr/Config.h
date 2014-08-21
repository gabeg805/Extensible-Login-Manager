#ifndef CONFIG_H
#define CONFIG_H


// Commands
extern char *DATE;
extern char *XORG;
extern char *XCOMPMGR;
extern char *HSETROOT;
extern char *XSETROOT;
extern char *AWK;
extern char *GREP;
extern char *TAIL;
extern char *LOGINCTL;
extern char *SYSTEMCTL;
extern char *POWEROFF;
extern char *REBOOT;
extern char *SESSREG;
extern char *PQIV;
extern char *XINITRC;

extern char *WM_SES_CMD;



// Position
extern int FRAME_XPOS;
extern int TEXTIMAGE_XPOS;
extern int USERNAME_XPOS;
extern int PASSWORD_XPOS;
extern int WINDOWMANAGER_XPOS;
extern int CLOCK_DATE_XPOS;
extern int CLOCK_TIME_XPOS;
extern int PANEL_SHUTDOWN_XPOS;
extern int PANEL_REBOOT_XPOS;
extern int PANEL_REFRESH_XPOS;
extern int PANEL_DIALOG_XPOS;

extern int FRAME_YPOS;
extern int TEXTIMAGE_YPOS;
extern int USERNAME_YPOS;
extern int PASSWORD_YPOS;
extern int WINDOWMANAGER_YPOS;
extern int CLOCK_DATE_YPOS;
extern int CLOCK_TIME_YPOS;
extern int PANEL_SHUTDOWN_YPOS;
extern int PANEL_REBOOT_YPOS;
extern int PANEL_REFRESH_YPOS;
extern int PANEL_DIALOG_YPOS;



// Size
extern int FRAME_WIDTH;
extern int USERNAME_WIDTH;
extern int PASSWORD_WIDTH;
extern int WINDOWMANAGER_WIDTH;
extern int CLOCK_DATE_HEIGHT;
extern int CLOCK_TIME_HEIGHT;
extern int PANEL_SHUTDOWN_WIDTH;
extern int PANEL_REBOOT_WIDTH;
extern int PANEL_REFRESH_WIDTH;

extern int FRAME_HEIGHT;
extern int USERNAME_HEIGHT;
extern int PASSWORD_HEIGHT;
extern int WINDOWMANAGER_HEIGHT;
extern int CLOCK_DATE_WIDTH;
extern int CLOCK_TIME_WIDTH;
extern int PANEL_SHUTDOWN_HEIGHT;
extern int PANEL_REBOOT_HEIGHT;
extern int PANEL_REFRESH_HEIGHT;



// Fonts
extern int TEXTIMAGE_FSIZE;
extern int USERNAME_FSIZE;
extern int PASSWORD_FSIZE;
extern int CLOCK_DATE_FSIZE;
extern int CLOCK_TIME_FSIZE;

extern char *TEXTIMAGE_FONT;
extern char *USERNAME_FONT;
extern char *PASSWORD_FONT;
extern char *CLOCK_DATE_FONT;
extern char *CLOCK_TIME_FONT;



// Utility Files
extern char *SERVICE;
extern char *WTMP;
extern char *UTMP_ADD;
extern char *UTMP_DEL;
extern char *XSERVER_AUTH;

// Log Files
extern char *GLM_LOG;
extern char *XSERVER_LOG;
extern char *TTY_LOG_DIR;
extern char *USERNAME_LOG;
extern char *SESSION_LOG;

// Image Files
extern char *WALLPAPER;
extern char *USERNAME_IMG;
extern char *WINDOWMANAGER_IMG;
extern char *PANEL_SHUTDOWN_IMG;
extern char *PANEL_REBOOT_IMG;
extern char *PANEL_REFRESH_IMG;



// Misc
extern char *TEXTIMAGE_TEXT;

extern char *CLOCK_DATE_FMT;
extern char *CLOCK_TIME_FMT;
extern int CLOCK_UPDATE_SEC;

extern char PASSWORD_INVISCHAR;
extern int PASSWORD_MAXCHARS;

extern int TTY_N;

extern char *DISPLAY;

extern int INTERFACE;
extern char *USERNAME;
extern char *PASSWORD;
extern char *SESSION;

extern int PREVIEW;

#endif
