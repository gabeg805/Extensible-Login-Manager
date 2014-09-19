#include <gtk/gtk.h>

#ifndef CONFIG_H
#define CONFIG_H



// Important
extern int PREVIEW;
extern int TTY_N;
extern int INTERFACE;
extern char *DISPLAY;
extern char *USERNAME;
extern char *PASSWORD;
extern char *SESSION;



// Miscellaneous
extern int PASSWORD_MAXCHARS;
extern char *TEXTIMAGE_TEXT;
extern char PASSWORD_INVISCHAR;


// Position
extern int TEXTIMAGE_XPOS;
extern int USERNAME_XPOS;
extern int PASSWORD_XPOS;
extern int WINDOWMANAGER_XPOS;
extern int PANEL_SHUTDOWN_XPOS;
extern int PANEL_REBOOT_XPOS;
extern int PANEL_REFRESH_XPOS;
extern int PANEL_DIALOG_XPOS;

extern int TEXTIMAGE_YPOS;
extern int USERNAME_YPOS;
extern int PASSWORD_YPOS;
extern int WINDOWMANAGER_YPOS;
extern int PANEL_SHUTDOWN_YPOS;
extern int PANEL_REBOOT_YPOS;
extern int PANEL_REFRESH_YPOS;
extern int PANEL_DIALOG_YPOS;



// Size
extern int USERNAME_WIDTH;
extern int PASSWORD_WIDTH;
extern int WINDOWMANAGER_WIDTH;
extern int PANEL_SHUTDOWN_WIDTH;
extern int PANEL_REBOOT_WIDTH;
extern int PANEL_REFRESH_WIDTH;

extern int USERNAME_HEIGHT;
extern int PASSWORD_HEIGHT;
extern int WINDOWMANAGER_HEIGHT;
extern int PANEL_SHUTDOWN_HEIGHT;
extern int PANEL_REBOOT_HEIGHT;
extern int PANEL_REFRESH_HEIGHT;



// Colors
const GdkRGBA BG_USERNAME;
const GdkRGBA BG_PASSWORD;
const GdkRGBA BG_SHUTDOWN;
const GdkRGBA BG_REBOOT;
const GdkRGBA BG_GLM_DIALOG;
const GdkRGBA BG_WINDOWMANAGER;

const GdkRGBA FG_USERNAME;
const GdkRGBA FG_PASSWORD;
const GdkRGBA FG_SHUTDOWN;
const GdkRGBA FG_REBOOT;
const GdkRGBA FG_GLM_DIALOG;
const GdkRGBA FG_WINDOWMANAGER;



// Fonts
extern int TEXTIMAGE_FSIZE;
extern int USERNAME_FSIZE;
extern int PASSWORD_FSIZE;

extern char *TEXTIMAGE_FONT;
extern char *USERNAME_FONT;
extern char *PASSWORD_FONT;



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


struct glmgui {
    GtkWidget *win;
    GtkWidget *widg;
    struct glmpos *pos;
    struct glmcolor *color;
    struct glmtext *text;
};


struct glmtext {
    char *font;
    int size;
    char *fmt;
};


struct glmcolor {
    GdkRGBA bgwin;
    GdkRGBA fgwin;
    GdkRGBA bgwidg;
    GdkRGBA fgwidg;
};


struct glmpos {
    int x;
    int y;
    int width;
    int height;
};


#endif
