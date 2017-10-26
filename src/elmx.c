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
#include "elmdef.h"
#include "elmio.h"
#include "utility.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pty.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xauth.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>

/* #include <dbus/dbus.h> */

/* Private functions */
static int elm_x_wait_sigusr(void);
static int elm_x_init(void);
static int elm_x_stop(Display *display);
static int elm_x_set_env(void);
static int elm_x_set_display_env(void);
static int elm_x_set_tty_env(void);
static int elm_x_set_ttyn_env(void);
static int elm_x_set_xauthority_env(void);
static int elm_x_set_xorgvt_env(void);
static int elm_x_get_vt(char *vt, size_t size);
static int elm_x_get_localhost(char *localhost, size_t size);
static int elm_x_get_xauth_file(char *file, size_t size);
static int elm_x_set_xauth_entry(char *filename, char *localhost);
static int elm_x_get_random_bytes(char **bytes, size_t size);
static int elm_x_is_running(void);

/* Private variables */
static Display *Xdisplay     = NULL;
static Window   Xwindow      = -1;
static pid_t    Xpid         = -1;
static int      ScreenWidth  = -1;
static int      ScreenHeight = -1;


/* ************************************************************************** */
/* Start the X server */
/* Note: I added a check if Xorg is already running, but if multiple instances 
 * can be run, it needs to be removed 
 */
int elm_x_run(void)
{
    elmprintf(LOGINFO, "Checking X server."); 

    /* Check if X server is already running */
    if (elm_x_is_running()) {
        elmprintf(LOGINFO, "X server already started.");

        if (elm_x_init() < 0) {
            exit(ELM_EXIT_X_INIT);
        }
   
        return 0;
    }

    elm_x_set_env();

    char *display    = getenv("DISPLAY");
    char *xauthority = getenv("XAUTHORITY");
    char *vt         = getenv("XORGVT");
    char *argv[]     = {ELM_CMD_XORG, display, "-background", "none",
                        "-noreset", "-verbose", "3", "-logverbose", "-logfile",
                        ELM_XLOG, "-auth", xauthority, "-seat", "seat0",
                        "-nolisten", "tcp", vt, NULL};

    /* Start X server */
    switch ((Xpid=fork()))
    {
    /* Child */
    case 0:
        elmprintf(LOGINFO, "Preparing to run X server.");

        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGUSR1, SIG_IGN);
        setpgid(0, getpid());

        elm_exec(ELM_CMD_XORG, argv);

        /* execl(ELM_CMD_XORG, ELM_CMD_XORG, */
        /*       display, */
        /*       "-background", "none", */
        /*       "-noreset", */
        /*       "-verbose", "3", "-logverbose", */
        /*       "-logfile", ELM_XLOG, */
        /*       "-auth", xauthority, */
        /*       "-seat", "seat0", */
        /*       "-nolisten", "tcp", */
        /*       vt, NULL); */

        /* elmprintf(LOGERRNO, "%s '%s'", "Error running", ELM_CMD_XORG); */

        exit(ELM_EXIT_X_RUN);

    /* Fork error */
    case -1:
        elmprintf(LOGERRNO, "%s '%s'",
                  "Error during fork to start", ELM_CMD_XORG);
        exit(ELM_EXIT_X_RUN);

    /* Parent */
    default:
        elmprintf(LOGINFO, "Preparing to initialize X server display."); 

        if (elm_x_wait_sigusr() < 0) {
            exit(ELM_EXIT_X_WAIT);
        }

        if (elm_x_init() < 0) {
            exit(ELM_EXIT_X_INIT);
        }
        break;
    }

    /* DBusError derr; */
    /* dbus_err_init(&derr); */
    /* DBusConnection *dcon = dbus_bus_get(DBUS_BUS_SYSTEM */

    return 0;
}

/* ************************************************************************** */
/* Wait for SIGUSR1 from Xorg, indicating it started successfully. Timeout in 30
 * seconds */
int elm_x_wait_sigusr(void)
{
    elmprintf(LOGINFO, "Setting up wait for 'SIGUSR1' signal.");

    struct timespec timeout = {30, 0};
    sigset_t        set;
    siginfo_t       info;

    if (sigemptyset(&set) < 0) {
        elmprintf(LOGERRNO, "Unable to empty signal set");
        return -1;
    }

    if (sigaddset(&set, SIGUSR1) < 0) {
        elmprintf(LOGERRNO, "Unable to add signal 'SIGUSR1' to set");
        return -2;
    }

    if (sigprocmask(SIG_BLOCK, &set, 0) != 0) {
        elmprintf(LOGERR, "Unable to set procmask.");
        return -3;
    }

    elmprintf(LOGINFO, "Waiting for X to send out 'SIGUSR1' signal.");

    if (sigtimedwait(&set, &info, &timeout) < 0) {
        elmprintf(LOGERRNO, "Unable to catch 'SIGUSR1' signal");
        return -4;
    }

    elmprintf(LOGINFO, "Done waiting for 'SIGUSR1' signal.");

    return 0;
}

/* ************************************************************************** */
/* Initialize X window attributes */
int elm_x_init(void)
{
    elmprintf(LOGINFO, "Preparing to open X server display.");

    /* Set X display */
    char *display = getenv("DISPLAY");

    if (!(Xdisplay=XOpenDisplay(display))) {
        elmprintf(LOGERRNO, "%s '%s'", "Unable to open display on", display);
        elm_x_stop(Xdisplay);
        return -1;
    }

    /* Set root window */
    Xwindow = DefaultRootWindow(Xdisplay);

    /* Set I/O error handler */
    XSetIOErrorHandler(elm_x_stop);

    return 0;
}

/* ************************************************************************** */
/* Stop X server */
int elm_x_stop(Display *display)
{
	elmprintf(LOGWARN, "Connection to X server lost.");

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
    elmprintf(LOGWARN, "Sending HUP to X server process group.");

	errno = 0;
	if((killpg(getpid(), SIGHUP) != 0) && (errno != ESRCH)) {
		elmprintf(LOGERRNO, "%s '%d'"
                  "Unable to send HUP to process group", getpid());
    }

	/* Send TERM to server */
    elmprintf(LOGWARN, "Sending TERM to X server process group.");

	if(Xpid < 0) {
        elmprintf(LOGERR, "X server has invalid pid '%lu'. Exiting.", Xpid);
    }
    else {
        errno = 0;
        if(killpg(Xpid, SIGTERM) < 0) {
            elmprintf(LOGERRNO, "Unable to terminate X server process group");
            exit(ELM_EXIT_X_STOP);
        }
    }

	/* Wait 10 sec for server to shut down */
    elmprintf(LOGWARN, "Waiting for X server to shut down.");

    int i;
    for (i=0; i < 10; i++)
    {
        if (waitpid(Xpid, 0, WNOHANG) == Xpid) {
            elmprintf(LOGWARN, "X server shutting down.");
            exit(ELM_EXIT_X_STOP);
        }

        sleep(1);
    }

	/* Send KILL to server */
    elmprintf(LOGWARN, "%s. %s", "X server is slow to shut down.",
              "Sending KILL to X server process group.");

	errno = 0;
	if(killpg(Xpid, SIGKILL) < 0) {
        elmprintf(LOGERRNO, "Unable to KILL X server process group");
	}

    /* Collect all dead children */
    elmprintf(LOGWARN, "Collecting all dead children.");

    while (waitpid(-1, 0, WNOHANG) > 0);

    exit(ELM_EXIT_X_STOP);
}

/* ************************************************************************** */
/* Set cursor */
int elm_x_set_cursor(void)
{
    elmprintf(LOGINFO, "Preparing to set cursor.");

    /* Set cursor */
    Cursor cursor = XCreateFontCursor(Xdisplay, XC_left_ptr);
    switch (cursor) {
    case BadAlloc:
        elmprintf(LOGERRNO, "Unable to create font cursor: Bad allocation");
        return -1;
    case BadValue:
        elmprintf(LOGERRNO, "Unable to create font cursor: Bad value");
        return -2;
    default:
        break;
    }

    int status = XDefineCursor(Xdisplay, Xwindow, cursor);
    switch (status) {
    case BadCursor:
        elmprintf(LOGERRNO, "Unable to define cursor: Bad cursor");
        return -3;
    case BadWindow:
        elmprintf(LOGERRNO, "Unable to define cursor: Bad window");
        return -4;
    default:
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
    elmprintf(LOGINFO, "Preparing to %s transparency.",
              (flag) ? "enable" : "disable");

    /* Do not run if flag not set */
    if (!flag) {
        return -1;
    }

    /* Check if transparency program is running */
    if (pgrep(basename(ELM_CMD_XCOMPMGR))) {
        elmprintf(LOGINFO, "X composite manager already started.");
        return -2;
    }

    /* Execute xcompmgr */
    char  *argv[] = {ELM_CMD_XCOMPMGR, NULL};
    pid_t  pid;

    switch ((pid=fork()))
    {
    /* Child */
    case 0:
        elm_exec(ELM_CMD_XCOMPMGR, argv);

        /* elmprintf(LOGINFO, "Running X composite manager."); */

        /* execl(ELM_CMD_XCOMPMGR, ELM_CMD_XCOMPMGR, NULL); */

        /* elmprintf(LOGERR, "%s '%s': %s.", */
        /*           "Error running", ELM_CMD_XCOMPMGR, strerror(errno)); */
        return 1;

    /* Fork failed */
    case -1:
        elmprintf(LOGERRNO, "%s '%s'",
                  "Error during fork to start", ELM_CMD_XCOMPMGR);
        return 2;

    /* Wait until process is ~probably~ fully started */
    default:
        waitpid(pid, 0, WNOHANG);
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
        snprintf(cmd, sizeof(cmd), "%s -merge %s", ELM_CMD_XRDB, xresources);
        elmprintf(LOGINFO, "Loading Xresources: '%s'.", cmd);
        system(cmd);
    }

    /* Load .Xmodmap */
    if (access(xmodmap, F_OK) == 0) {
        snprintf(cmd, sizeof(cmd), "%s %s", ELM_CMD_XMODMAP, xmodmap);
        elmprintf(LOGINFO, "Loading Xmodmap: '%s'.", cmd);
        system(cmd);
    }

    return 0;
}

/* ************************************************************************** */
/* Set environment variables */
int elm_x_set_env(void)
{
    elmprintf(LOGINFO, "Setting up X server environment variables."); 

    if (elm_x_set_display_env() < 0) {
        exit(ELM_EXIT_X_ENV_DISPLAY);
    }

    if (elm_x_set_tty_env() < 0) {
        exit(ELM_EXIT_X_ENV_TTY);
    }

    if (elm_x_set_ttyn_env() < 0) {
        exit(ELM_EXIT_X_ENV_TTYN);
    }

    if (elm_x_set_xauthority_env() < 0) {
        exit(ELM_EXIT_X_ENV_XAUTH);
    }

    if (elm_x_set_xorgvt_env() < 0) {
        exit(ELM_EXIT_X_ENV_VT);
    }

    return 0;
}

/* ************************************************************************** */
/* Set DISPLAY environment variable */
int elm_x_set_display_env(void)
{
    elmprintf(LOGINFO, "Setting DISPLAY environment variable.");

    if (getenv("DISPLAY")) {
        elmprintf(LOGINFO, "%s: '%s=%s'.",
                  "Environment variable already set", "DISPLAY=",
                  getenv("DISPLAY"));
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
        elmprintf(LOGERR, "Unable to find an open display.");
        return -1;
    }

    /* Set display environment variable */
    char display[3] = {':', i+'0', 0};

    if (elm_setenv("DISPLAY", display) < 0) {
        return -2;
    }

    return 0;
}

/* ************************************************************************** */
/* Set currently used tty */
int elm_x_set_tty_env(void)
{
    /* Open tty file descriptor */
    int   master;
    int   slave;
    char *tty;

    if (openpty(&master, &slave, NULL, NULL, NULL) < 0) {
        elmprintf(LOGERRNO, "Error finding open tty.");
        return -1;
    }

    /* Get tty path from file descriptor */
    if (!(tty=ttyname(slave))) {
        elmprintf(LOGERR, "%s", "Error finding terminal device path name");
        return -2;
    }

    /* Set tty environment variable */
    if (elm_setenv("TTY", tty) < 0) {
        return -3;
    }

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
    ttyn = (strstr(tty, "pts")) ? ttyn+2 : ttyn;
    /* if (strstr(tty, "pts")) */
    /*     ttyn += 2; */

    /* Check tty number */
    if ((ttyn <= 0) || (ttyn > 9)) {
        elmprintf(LOGERR, "Invalid number from tty '%s'.", tty);
        return -1;
    }

    /* Set ttyn environment variable */
    char str[2] = {ttyn+'0', 0};

    if (elm_setenv("TTYN", str) < 0) {
        return -2;
    }

    return 0;
}

/* ************************************************************************** */
/* Set XAUTHORITY environment variable */
int elm_x_set_xauthority_env(void)
{
    char localhost[HOST_NAME_MAX+1];
    char authfile[64];

    if (elm_x_get_localhost(localhost, sizeof(localhost)-1) < 0) {
        return -1;
    }

    if (elm_x_get_xauth_file(authfile, sizeof(authfile)-1) < 0) {
        return -2;
    }

    if (elm_x_set_xauth_entry(authfile, localhost) < 0) {
        return -3;
    }

    if (elm_setenv("XAUTHORITY", authfile) < 0) {
        return -4;
    }

    return 0;
}

/* ************************************************************************** */
/* Set XORGVT environment variable. Note: This is not a standard environment
 * variable but I am setting it to keep the X run function consistent.
 */
int  elm_x_set_xorgvt_env(void)
{
    char vt[5];
    elm_x_get_vt(vt, sizeof(vt));

    if (elm_setenv("XORGVT", vt) < 0) {
        exit(101);
    }

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
        elmprintf(LOGERR, "Unable to set screen dimensions.");
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
            elmprintf(LOGINFO, "%s: '%d x %d'.",
                      "Set screen width x height", ScreenWidth, ScreenHeight);
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
/* Return the localhost for the Xauthority entry */
int elm_x_get_localhost(char *localhost, size_t size)
{
    if (gethostname(localhost, size)) {
        elmprintf(LOGERR, "Unable to 'gethostname'.");
        strncpy(localhost, "localhost", size);
    }

    elmprintf(LOGINFO, "X authority localhost: %s.", localhost); 

    return 0;
}

/* ************************************************************************** */
/* Return the Xauthority file path */
int elm_x_get_xauth_file(char *file, size_t size)
{
    /* char *home = getenv("HOME"); */

    /* if (home) { */
    /*     snprintf(file, size, "%s/.Xauthority", home); */
    /* } */
    /* else { */
        if (access(ELM_VAR_RUN_DIR, W_OK) == 0) {
            snprintf(file, size, "%s/.Xauthority", ELM_VAR_RUN_DIR);
        }
        else {
            snprintf(file, size, "/tmp/.Xauthority");
        }
    /* } */

    elmprintf(LOGINFO, "X authority file: '%s'.", file); 

    return 0;
}

/* ************************************************************************** */
/* Return the Xauthority entry */
int elm_x_set_xauth_entry(char *filename, char *localhost)
{
    Xauth entry = {0};
    entry.family         = FamilyLocal;
    entry.address        = localhost;
    entry.address_length = strlen(entry.address);
    entry.name           = "MIT-MAGIC-COOKIE-1";
    entry.name_length    = strlen(entry.name);
    entry.data_length    = 16;

    if (elm_x_get_random_bytes(&entry.data, entry.data_length) < 0) {
        return -1;
    }

    /* Open file for writing */
    int   fd;
    FILE *handle;

    if ((fd=open(filename, (O_RDWR | O_CREAT | O_TRUNC), 0700)) < 0) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to open Xauthority file", filename);
        free(entry.data);
        return -2;
    }

    if (!(handle=fdopen(fd, "w+"))) {
        elmprintf(LOGERR, "%s '%d'",
                  "Unable to open Xauthority file descriptor", fd);
        free(entry.data);
        close(fd);
        return -3;
    }

    /* Write to xauthority file */
    if (!XauWriteAuth(handle, &entry) || fflush(handle)) {
        elmprintf(LOGERR, "%s: '%s'.",
                  "Unable to write to local Xauthority file", filename);
        free(entry.data);
        close(fd);
        fclose(handle);
        return -4;
    }

    entry.family = FamilyWild;
    if (!XauWriteAuth(handle, &entry) || fflush(handle)) {
        elmprintf(LOGERR, "%s: '%s'.",
                  "Unable to write to wild Xauthority file", filename);
        free(entry.data);
        close(fd);
        fclose(handle);
        return -5;
    }

    free(entry.data);
    close(fd);
    fclose(handle);

    return 0;
}

/* ************************************************************************** */
/* Return the VT to use in Xorg command */
int elm_x_get_vt(char *vt, size_t size)
{
    snprintf(vt, size, "vt%s", getenv("TTYN"));

    elmprintf(LOGINFO, "Xorg VT: %s.", vt);

    return 0;
}

/* ************************************************************************** */
/* Return random number of bytes */
int elm_x_get_random_bytes(char **bytes, size_t size)
{
    char randfile[] = "/dev/urandom";
    int  fd         = open(randfile, O_RDONLY);

    /* Check urandom file */
    if (fd < 0) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to open file descriptor for", randfile);
        return -1;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        elmprintf(LOGERRNO, "Unable to seek file descriptor");
        return -2;
    }

    /* Copy from urandom */
    *bytes = malloc(size);
    if (read(fd, *bytes, size) < 0) {
        elmprintf(LOGERRNO, "Unable to read file descriptor");
        return -3;
    }

    close (fd);

    return 0;
}

/* ************************************************************************** */
/* Check if X is running */
int elm_x_is_running(void)
{
    return (getenv("DISPLAY")) ? 1 : 0;
}
