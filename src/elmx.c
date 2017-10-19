/* *****************************************************************************
 * 
 * Name:    elmx.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Setup the X server.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmx.h"
#include "elmio.h"
#include "utility.h"
#include <ctype.h>
#include <errno.h>
#include <pty.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>

/* Private functions */
static void    xstop(void);
static void    xrestart(void);
static int     xtimeout(uint8_t timeout);
static int     xwait(void);
static int     xignoreio(Display *display);
static int     xioerror(Display *display);

static int elm_x_is_running(void);
static int elm_x_set_display_env(void);
static int elm_x_set_tty_env(void);
static int elm_x_set_ttyn_env(void);

/* Private variables */
static Display *Xdisplay   = NULL;
static Window   Xwindow    = -1;
static pid_t    Xpid       = -1;
static int      ScreenWidth  = -1;
static int      ScreenHeight = -1;

/* Not sure what this does (Slim) */
jmp_buf xcloseenv;

/* ************************************************************************** */
/* Initialize X window attributes */
int elm_x_init(void)
{
    elmprintf(LOG, "Opening X display.");

    /* Set X display */
    char *display = getenv("DISPLAY");

    if (!(Xdisplay=XOpenDisplay(display))) {
        elmprintf(LOG, "Unabled to open display on '%s': %s.", display,
                  strerror(errno));
        /* xstop(); */
        exit(ELM_EXIT_X_OPEN);
    }

    /* Set root window */
    Xwindow = DefaultRootWindow(Xdisplay);

    return 0;
}

/* ************************************************************************** */
/* Start the X server */
/* Note: I added a check if Xorg is already running, but if multiple instances 
 * can be run, it needs to be removed 
 */
int elm_x_start(void)
{
    elmprintf(LOG, "X server is starting.");
    if (elm_x_is_running()) {
        elmprintf(LOG, "X server already started.");
        return 0;
    }

    /* Setup X environment variables */
    elm_x_set_display_env();
    elm_x_set_tty_env();
    elm_x_set_ttyn_env();

    /* Start X server */
    Xpid = fork();
    switch (Xpid)
    {
    /* Child */
    case 0:
        signal(SIGTTIN, SIG_IGN); /* Slim */
        signal(SIGTTOU, SIG_IGN); /* Slim */
        signal(SIGUSR1, SIG_IGN); /* Slim */
        setpgid(0, getpid());     /* Slim */

        const char *xorg     = "/usr/bin/Xorg";
        const char *authfile = "/tmp/elm.auth";
        const char *logfile  = "/var/log/xserver.log";
        char vt[5];
        snprintf(vt, sizeof(vt), "vt%s", getenv("TTYN"));

        execl(xorg, xorg,
              /* "-background", "none", */
              /* "-noreset", */
              /* "-keeptty", */
              "-nolisten", "tcp",
              "-seat", "seat0",
              "-auth", authfile,
              "-logverbose", "-logfile", logfile,
              getenv("DISPLAY"), vt, NULL);

        elmprintf(LOG, "Error running Xorg: %s.", strerror(errno));
        exit(ELM_EXIT_X_START);
        break;

    /* Fork error */
    case -1:
        elmprintf(LOG, "Error forking to start Xorg: %s.", strerror(errno));
        exit(ELM_EXIT_X_START);
        break;

    /* Parent */
    default:
        sleep(1);
        if(xtimeout(0)) {
            Xpid = -1;
            return Xpid;
        }

        /* Wait for server to start up */
        if(xwait()) {
            xstop();
            Xpid = -1;
            exit(ELM_EXIT_X_WAIT);
        }
        break;
    }

    return 0;
}

/* ************************************************************************** */
/* Stop X server */
void xstop(void)
{
    elmprintf(LOG, "Stopping X server.");
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, SIG_DFL);
	signal(SIGKILL, SIG_DFL);

	/* Catch X error */
    elmprintf(LOG, "Catching X server error.");

	XSetIOErrorHandler(xignoreio);
	if(!setjmp(xcloseenv) && Xdisplay)
		XCloseDisplay(Xdisplay);

	/* Send HUP to process group */
    elmprintf(LOG, "Sending HUP to X server process group.");

	errno = 0;
	if((killpg(getpid(), SIGHUP) != 0) && (errno != ESRCH))
		elmprintf(LOG, "Unable to send HUP to process group '%d'.", getpid());

	/* Send TERM to server */
    elmprintf(LOG, "Sending TERM to X server process group.");

	if(Xpid < 0) {
        elmprintf(LOG, "X server has invalid pid '%lu'. Exiting.", Xpid);
		return;
    }

	errno = 0;
	if(killpg(Xpid, SIGTERM) < 0) {
		if(errno == EPERM) {
			elmprintf(LOG, "Unable to terminate X server process group.");
			exit(ELM_EXIT_X_STOP);
		}
		if(errno == ESRCH)
			return;
	}

	/* Wait for server to shut down */
    elmprintf(LOG, "Waiting for X server to shut down.");

	if(xtimeout(10)) {
        elmprintf(LOG, "X server shutting down.");
		return;
    }
    elmprintf(LOG, "X server is slow to shut down. Sending KILL signal.");

	/* Send KILL to server */
    elmprintf(LOG, "Sending KILL to X server process group.");

	errno = 0;
	if(killpg(Xpid, SIGKILL) < 0) {
		if(errno == ESRCH)
			return;
	}

	/* Wait for server to die */
	if(!xtimeout(3)) {
		elmprintf(LOG, "Unable to kill server");
		exit(ELM_EXIT_X_STOP);
	}
}

/* ************************************************************************** */
/* Restart X server */
void xrestart(void)
{
    elmprintf(LOG, "X server is restarting.");

    xstop();
    while (waitpid(-1, NULL, WNOHANG) > 0); /* Collect all dead children */
    elm_x_start();
    elm_x_init();
}

/* ************************************************************************** */
/* Wait for server to timeout */
int xtimeout(uint8_t timeout)
{
    pid_t   pid;
    uint8_t i;

    for (i=0; i <= timeout; ++i) {
        if ((pid=waitpid(Xpid, NULL, WNOHANG)) == Xpid)
            return 1;
        if (timeout)
            sleep(1);
    }

    return 0;
}

/* ************************************************************************** */
/* Wait for server to start */
int xwait(void)
{
    char    *display  = getenv("DISPLAY");
    uint8_t  ntimeout = 120;
    uint8_t  i;

    for (i=0; i < ntimeout; ++i) {
        if ((Xdisplay=XOpenDisplay(display))) {
            XSetIOErrorHandler(xioerror);
            return 0;
        }
        else {
            if (xtimeout(1))
                break;
        }
    }

    return 1;
}

/* ************************************************************************** */
/* Ignore X server I/O */
int xignoreio(Display *display) {
	elmprintf(LOG, "Connection to X server lost.");
	longjmp(xcloseenv, 1);
}

/* ************************************************************************** */
/* Handle X input/output errors */
int xioerror(Display *display)
{
    xrestart();
    return 0;
}

/* ************************************************************************** */
/* Set cursor */
int elm_x_set_cursor(void)
{
    /* Set cursor */
    Cursor cursor = XCreateFontCursor(Xdisplay, XC_left_ptr);
    switch (cursor) {
    case BadAlloc:
        elmprintf(LOG, "Unable to create font cursor: Bad allocation: '%s'.", strerror(errno));
        return 1;
    case BadValue:
        elmprintf(LOG, "Unable to create font cursor: Bad value: '%s'.", strerror(errno));
        return 1;
    default:
        elmprintf(LOG, "Creating font cursor.");
        break;
    }

    int status = XDefineCursor(Xdisplay, Xwindow, cursor);
    switch (status) {
    case BadCursor:
        elmprintf(LOG, "Unable to define cursor: Bad cursor: '%s'.", strerror(errno));
        return 1;
    case BadWindow:
        elmprintf(LOG, "Unable to define cursor: Bad window: '%s'.", strerror(errno));
        return 1;
    default:
        elmprintf(LOG, "Defining font cursor.");
        break;
    }

    /* Flush events to X server */
    XFlush(Xdisplay);

    return 0;
}

/* ************************************************************************** */
/* Start compositing manager (for transparency) */
int elm_x_set_transparency(int flag)
{
    /* Do not run if flag not set */
    if (!flag)
        return 1;

    /* Check if transparency program is running */
    const char *xcompmgr = "/usr/bin/xcompmgr";

    if (pgrep(basename(xcompmgr))) {
        elmprintf(LOG, "X composite manager already started.");
        return 1;
    }

    /* Execute xcompmgr */
    pid_t pid = fork();

    switch (pid)
    {
    /* Child */
    case 0:
        execl(xcompmgr, xcompmgr, NULL);
        elmprintf(LOG, "Error exec-ing %s: %s", xcompmgr, strerror(errno));
        exit(ELM_EXIT_X_XCOMPMGR);
        break;

    /* Fork failed */
    case -1:
        elmprintf(LOG, "Error forking to start %s: %s.", xcompmgr,
                  strerror(errno));
        return -1;

    /* Wait until process is ~probably~ fully started */
    default:
        waitpid(pid, NULL, WNOHANG);
        sleep(2);
        break;
    }

    return 0;
}

/* ************************************************************************** */
/* Load preferences from Xresources and Xmodmap */
int elm_x_load_user_preferences(void)
{
    char *home = getenv("HOME");
    char  xresources[64];
    char  xmodmap[64];
    char  cmd[128];

    snprintf(xresources, sizeof(xresources), "%s/.Xresources", home);
    snprintf(xmodmap,    sizeof(xmodmap),    "%s/.Xmodmap",    home);

    /* Load .Xresources */
    if (access(xresources, F_OK) == 0) {
        snprintf(cmd, sizeof(cmd), "/usr/bin/xrdb -merge %s", xresources);
        elmprintf(LOG, "Executing: %s", cmd);
        system(cmd);
    }

    /* Load .Xmodmap */
    if (access(xmodmap, F_OK) == 0) {
        snprintf(cmd, sizeof(cmd), "/usr/bin/xmodmap %s", xmodmap);
        elmprintf(LOG, "Executing: %s", cmd);
        system(cmd);
    }

    return 0;
}

/* ************************************************************************** */
/* Set DISPLAY environment variable */
int elm_x_set_display_env(void)
{
    elmprintf(LOG, "Setting DISPLAY environment variable.");
    if (getenv("DISPLAY")) {
        elmprintf(LOG, "DISPLAY environment variable already set.");
        return 1;
    }

    /* Find an open display */
    char file[15];
    int  val = -1;
    int  i;

    for (i=0; i < 10; i++) {
        snprintf(file, sizeof(file), "/tmp/.X%d-lock", i);
        if (access(file, F_OK)) {
            val = i;
            break;
        }
    }

    /* Check display value */
    if (val < 0) {
        elmprintf(LOG, "Unable to find an open display.");
        exit(ELM_EXIT_X_DISPLAY);
    }

    /* Set display environment variable */
    char display[3] = {':', i+'0', 0};

    if (elm_setenv("DISPLAY", display) < 0)
        exit(ELM_EXIT_X_DISPLAY);

    return 0;
}

/* ************************************************************************** */
/* Set currently used tty */
int elm_x_set_tty_env(void)
{
    /* Open tty file descriptor */
    int   master;
    int   slave;

    if (openpty(&master, &slave, NULL, NULL, NULL) < 0) {
        elmprintf(LOG, "Error finding open tty: %s.", strerror(errno));
        exit(ELM_EXIT_X_TTY);
    }

    /* Get tty path from file descriptor */
    char *tty = ttyname(slave);

    if (!tty) {
        elmprintf(LOG, "Error finding terminal device path name: %s.",
                  strerror(errno));
        exit(ELM_EXIT_X_TTY);
    }

    /* Set tty environment variable */
    if (elm_setenv("TTY", tty) < 0)
        exit(ELM_EXIT_X_TTY);

    return 0;
}

/* ************************************************************************** */
/* Set TTYN environment variable */
int elm_x_set_ttyn_env(void)
{
    /* Get tty number from name in path */
    char   *tty    = getenv("TTY");
    int     ttyn   = 0;
    size_t  shift  = 0;
    size_t  length = strlen(tty);

    while ((!isdigit(tty[shift])) && (shift < length))
        ++shift;
    ttyn = atoi(tty+shift);
    /* ttyn = (strstr(tty, "pts")) ? ttyn+2 : ttyn; */
    if (strstr(tty, "pts"))
        ttyn += 2;

    /* Check tty number */
    if ((ttyn <= 0) || (ttyn > 9)) {
        elmprintf(LOG, "Invalid number from tty '%s'.", tty);
        exit(ELM_EXIT_X_TTY);
    }

    /* Set ttyn environment variable */
    char str[2] = {ttyn+'0', 0};

    if (elm_setenv("TTYN", str) < 0)
        exit(ELM_EXIT_X_TTY);

    return 0;
}

/* ************************************************************************** */
/* Set screen width and height. These are the dimensions of the active
 * monitor.
 */
int elm_x_set_screen_dimensions(void)
{
    XRRScreenResources *screen = XRRGetScreenResources(Xdisplay, DefaultRootWindow(Xdisplay));
    ScreenWidth  = -1;
    ScreenHeight = -1;

    /* Unable to determine screen info */
    if (!screen) {
        elmprintf(LOG, "Unable to set screen dimensions.");
        return 1;
    }

    ScreenWidth  = 0;
    ScreenHeight = 0;
    int          num = screen->ncrtc;
    int          i;
    XRRCrtcInfo *info;

    /* Set screen dimensions for monitor at x=0 */
    for (i=0; i < num; i++) {
        info = XRRGetCrtcInfo(Xdisplay, screen, screen->crtcs[i]);

        if (info->x == 0) {
            ScreenWidth  = info->width;
            ScreenHeight = info->height;
            XRRFreeCrtcInfo(info);
            break;
        }

        XRRFreeCrtcInfo(info);
    }

    XRRFreeScreenResources(screen);

    return 0;
}

/* ************************************************************************** */
/* Return screen width */
int elm_x_get_screen_width(void)
{
    return ScreenWidth;
}

/* ************************************************************************** */
/* Return screen height */
int elm_x_get_screen_height(void)
{
    return ScreenHeight;
}

/* ************************************************************************** */
/* Check if X is running */
int elm_x_is_running(void)
{
    return (getenv("DISPLAY")) ? 1 : 0;
}

    /* GdkDisplay   *dsp = gdk_display_get_default(); */
    /* GdkMonitor   *mon; */
    /* GdkRectangle  rect; */
    /* int           num_monitors = gdk_display_get_n_monitors(dsp); */
    /* int           i; */

    /* for (i = 0; i < num_monitors; i++) { */
    /*     mon = gdk_display_get_monitor(dsp, i); */
    /*     gdk_monitor_get_geometry(mon, &rect); */
    /*     printf ("monitor %d: offsets (%d, %d), size (%d, %d)\n", */
    /*             i, rect.x, rect.y, rect.width, rect.height); */
    /* } */

