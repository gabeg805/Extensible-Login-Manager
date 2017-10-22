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

/* #include <dbus/dbus.h> */

/* Private functions */
static void elm_x_init(void);
static void elm_x_wait_startup(void);
static int  elm_x_stop(Display *display);
static int  elm_x_set_display_env(void);
static int  elm_x_set_tty_env(void);
static int  elm_x_set_ttyn_env(void);
static int  elm_x_is_running(void);

/* Private variables */
static Display *Xdisplay   = NULL;
static Window   Xwindow    = -1;
static pid_t    Xpid       = -1;
static int      ScreenWidth  = -1;
static int      ScreenHeight = -1;

/* Not sure what this does (Slim) */
jmp_buf xcloseenv;

/* ************************************************************************** */
/* Start the X server */
/* Note: I added a check if Xorg is already running, but if multiple instances 
 * can be run, it needs to be removed 
 */
int elm_x_run(void)
{
    elmprintf(LOG, "X server is starting.");

    if (elm_x_is_running()) {
        elmprintf(LOG, "X server already started.");
        elm_x_init();
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
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGUSR1, SIG_IGN);
        setpgid(0, getpid());     /* Slim */

        const char *xorg     = "/usr/bin/Xorg";
        const char *authfile = "/tmp/elm.auth";
        const char *logfile  = "/var/log/xserver.log";
        char vt[5];
        snprintf(vt, sizeof(vt), "vt%s", getenv("TTYN"));

       /* └─287 /usr/bin/Xorg.bin :0 -background none -noreset -verbose 3 
        * -logfile /dev/null -auth /var/run/gdm/auth-for-gdm-4X6qTS/database 
        * -seat seat0 -nolisten tcp vt1 */
        /* create var run directory for elm? */
        /* create var log directory for elm? */

        execl(xorg, xorg,
              getenv("DISPLAY"),
              "-background", "none",
              "-noreset",
              "-verbose", "3", "-logverbose",
              "-logfile", logfile,
              "-auth", authfile,
              "-seat", "seat0",
              "-nolisten", "tcp",
              vt, NULL);

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
        elm_x_wait_startup();
        elm_x_init();
        break;
    }


    /* DBusError derr; */
    /* dbus_err_init(&derr); */

    /* DBusConnection *dcon = dbus_bus_get(DBUS_BUS_SYSTEM */


    return 0;
}

/* ************************************************************************** */
/* Initialize X window attributes */
void elm_x_init(void)
{
    elmprintf(LOG, "Opening X display.");

    /* Set X display */
    char *display = getenv("DISPLAY");

    if (!(Xdisplay=XOpenDisplay(display))) {
        elmprintf(LOG, "Unabled to open display on '%s': %s.", display,
                  strerror(errno));
        elm_x_stop(Xdisplay);
        exit(ELM_EXIT_X_OPEN);
    }

    /* Set root window */
    Xwindow = DefaultRootWindow(Xdisplay);

    /* Set I/O error handler */
    XSetIOErrorHandler(elm_x_stop);
}

/* ************************************************************************** */
/* Wait for SIGUSR1 from Xorg, indicating it started successfully. Timeout in 30
 * seconds */
void elm_x_wait_startup(void)
{
    elmprintf(LOG, "Waiting for X to startup.");

    const struct timespec timeout = {30, 0};
    sigset_t  set;
    siginfo_t info;

    if (sigemptyset(&set)) {
        elmprintf(LOG, "Unable to empty signal set: %s.", strerror(errno));
        exit(ELM_EXIT_X_WAIT);
    }

    if (sigaddset(&set, SIGUSR1)) {
        elmprintf(LOG, "Unable to add signal to set: %s.", strerror(errno));
        exit(ELM_EXIT_X_WAIT);
    }

    /* Wait for signal */
    if (sigtimedwait(&set, &info, &timeout) <= 0) {
        elmprintf(LOG, "Error waiting for signal: %s.", strerror(errno));
        exit(ELM_EXIT_X_WAIT);
    }
}



/* ************************************************************************** */
/* Stop X server */
int elm_x_stop(Display *display)
{
	elmprintf(LOG, "Connection to X server lost.");

	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTERM, SIG_DFL);
	signal(SIGKILL, SIG_DFL);

	/* Close display */
	if(Xdisplay) {
		XCloseDisplay(Xdisplay);
    }

	/* Send HUP to process group */
    elmprintf(LOG, "Sending HUP to X server process group.");

	errno = 0;
	if((killpg(getpid(), SIGHUP) != 0) && (errno != ESRCH)) {
		elmprintf(LOG, "Unable to send HUP to process group '%d'.", getpid());
    }

	/* Send TERM to server */
    elmprintf(LOG, "Sending TERM to X server process group.");

	if(Xpid < 0) {
        elmprintf(LOG, "X server has invalid pid '%lu'. Exiting.", Xpid);
    }
    else {
        errno = 0;
        if(killpg(Xpid, SIGTERM) < 0) {
            elmprintf(LOG, "Unable to terminate X server process group: %s.",
                      strerror(errno));
            exit(ELM_EXIT_X_STOP);
        }
    }

	/* Wait 10 sec for server to shut down */
    elmprintf(LOG, "Waiting for X server to shut down.");

    int i;
    for (i=0; i < 10; i++)
    {
        if (waitpid(Xpid, NULL, WNOHANG) == Xpid) {
            elmprintf(LOG, "X server shutting down.");
            exit(ELM_EXIT_X_STOP);
        }

        sleep(1);
    }

	/* Send KILL to server */
    elmprintf(LOG, "%s. %s", "X server is slow to shut down.",
              "Sending KILL to X server process group.");

	errno = 0;
	if(killpg(Xpid, SIGKILL) < 0) {
        elmprintf(LOG, "Unable to KILL X server process group: %s.",
                  strerror(errno));
	}

    /* Collect all dead children */
    elmprintf(LOG, "Collecting all dead children.");

    while (waitpid(-1, NULL, WNOHANG) > 0);

    exit(ELM_EXIT_X_STOP);
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
    Window              window = DefaultRootWindow(Xdisplay);
    XRRScreenResources *screen = XRRGetScreenResources(Xdisplay, window);
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
            elmprintf(LOG, "Set screen Width x Height: '%d x %d'.", ScreenWidth, ScreenHeight);
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
