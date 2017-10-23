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
static void elm_x_sigusr(int sig, siginfo_t *info, void *context);
static void elm_x_wait_sigusr(void);
static void elm_x_init(void);
static int  elm_x_stop(Display *display);
static int  elm_x_set_display_env(void);
static int  elm_x_set_tty_env(void);
static int  elm_x_set_ttyn_env(void);
static int  elm_x_set_xauthority_env(void);
static int  elm_x_is_running(void);

static int elm_x_get_vt(char *vt, size_t size);
static int elm_x_get_localhost(char *localhost, size_t size);
static int elm_x_get_xauth_file(char *file, size_t size);
static int elm_x_set_xauth_entry(char *filename, char *localhost);
static int elm_x_get_random_bytes(char **bytes, size_t size);

/* Private variables */
static Display *Xdisplay     = NULL;
static Window   Xwindow      = -1;
static pid_t    Xpid         = -1;
static int      Xsigusr      = 0;
static int      ScreenWidth  = -1;
static int      ScreenHeight = -1;


/* ************************************************************************** */
/* Start the X server */
/* Note: I added a check if Xorg is already running, but if multiple instances 
 * can be run, it needs to be removed 
 */
int elm_x_run(void)
{
    elmprintf(LOGINFO, "Preparing to run X server."); 

    if (elm_x_is_running()) {
        elmprintf(LOGINFO, "X server already started.");
        elm_x_init();
        return 0;
    }

    /* Setup X environment variables */
    elm_x_set_display_env();
    elm_x_set_tty_env();
    elm_x_set_ttyn_env();
    elm_x_set_xauthority_env();

    elmprintf(LOGINFO, "XAUTHORITY environment variable is now set: %s.", getenv("XAUTHORITY"));

    /* Handle SIGUSR1 signal, sent out by Xorg when server is ready */
    struct sigaction act;
    act.sa_flags     = SA_SIGINFO;
    act.sa_sigaction = &elm_x_sigusr;

    sigaction(SIGUSR1, &act, NULL);

    /* Start X server */
    switch ((Xpid=fork()))
    {
    /* Child */
    case 0:
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGUSR1, SIG_IGN);
        setpgid(0, getpid());

        elmprintf(LOGINFO, "Setting up X authority file."); 

        char vt[5];
        elm_x_get_vt(vt, 5);

        /* snprintf(authfile, sizeof(authfile), "/tmp/elmxauth"); */

        elmprintf(LOGINFO, "Running X server.");

        execl(ELM_CMD_XORG, ELM_CMD_XORG,
              getenv("DISPLAY"),
              "-background", "none",
              "-noreset",
              "-verbose", "3", "-logverbose",
              "-logfile", ELM_XLOG,
              "-auth", getenv("XAUTHORITY"),
              "-seat", "seat0",
              "-nolisten", "tcp",
              vt, NULL);

        elmprintf(LOGERR, "Error running '%s': %s.", ELM_CMD_XORG, strerror(errno));
        exit(ELM_EXIT_X_START);
        break;

    /* Fork error */
    case -1:
        elmprintf(LOGERR, "Error forking to start '%s': %s.", ELM_CMD_XORG,
                  strerror(errno));
        exit(ELM_EXIT_X_START);
        break;

    /* Parent */
    default:
        elm_x_wait_sigusr();
        elm_x_init();
        break;
    }


    /* DBusError derr; */
    /* dbus_err_init(&derr); */

    /* DBusConnection *dcon = dbus_bus_get(DBUS_BUS_SYSTEM */


    return 0;
}

/* ************************************************************************** */
/* Catch SIGUSR1 signal sent out by Xorg to indicate X is ready */
void elm_x_sigusr(int sig, siginfo_t *info, void *context)
{
    elmprintf(LOGINFO, "Caught SIGUSR1 signal (%d).", info->si_signo);
    Xsigusr = 1;
}

/* ************************************************************************** */
/* Wait for SIGUSR1 from Xorg, indicating it started successfully. Timeout in 30
 * seconds */
void elm_x_wait_sigusr(void)
{
    elmprintf(LOGINFO, "Waiting for X to send out SIGUSR1 signal.");

    int timeout = 30;
    int sec     = 0;

    while ((sec < timeout) && !Xsigusr) {
        sleep(1);
        sec++;
    }

    if (!Xsigusr) {
        elmprintf(LOGINFO, "%s: %s",
                  "X server is unable to be used",
                  "SIGUSR signal was never send/received.");
        exit(ELM_EXIT_X_WAIT);
    }
}

/* ************************************************************************** */
/* Initialize X window attributes */
void elm_x_init(void)
{
    elmprintf(LOGINFO, "Preparing to open X display.");

    /* Set X display */
    char *display = getenv("DISPLAY");

    if (!(Xdisplay=XOpenDisplay(display))) {
        elmprintf(LOGERR, "Unable to open display on '%s': %s.", display,
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
		elmprintf(LOGERR, "Unable to send HUP to process group '%d'.", getpid());
    }

	/* Send TERM to server */
    elmprintf(LOGWARN, "Sending TERM to X server process group.");

	if(Xpid < 0) {
        elmprintf(LOGERR, "X server has invalid pid '%lu'. Exiting.", Xpid);
    }
    else {
        errno = 0;
        if(killpg(Xpid, SIGTERM) < 0) {
            elmprintf(LOGERR, "Unable to terminate X server process group: %s.",
                      strerror(errno));
            exit(ELM_EXIT_X_STOP);
        }
    }

	/* Wait 10 sec for server to shut down */
    elmprintf(LOGWARN, "Waiting for X server to shut down.");

    int i;
    for (i=0; i < 10; i++)
    {
        if (waitpid(Xpid, NULL, WNOHANG) == Xpid) {
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
        elmprintf(LOGERR, "Unable to KILL X server process group: %s.",
                  strerror(errno));
	}

    /* Collect all dead children */
    elmprintf(LOGWARN, "Collecting all dead children.");

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
        elmprintf(LOGERR, "Unable to create font cursor: Bad allocation: '%s'.", strerror(errno));
        return 1;
    case BadValue:
        elmprintf(LOGERR, "Unable to create font cursor: Bad value: '%s'.", strerror(errno));
        return 1;
    default:
        elmprintf(LOGINFO, "Creating font cursor.");
        break;
    }

    int status = XDefineCursor(Xdisplay, Xwindow, cursor);
    switch (status) {
    case BadCursor:
        elmprintf(LOGERR, "Unable to define cursor: Bad cursor: '%s'.", strerror(errno));
        return 1;
    case BadWindow:
        elmprintf(LOGERR, "Unable to define cursor: Bad window: '%s'.", strerror(errno));
        return 1;
    default:
        elmprintf(LOGINFO, "Defining font cursor.");
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
    if (pgrep(basename(ELM_CMD_XCOMPMGR))) {
        elmprintf(LOGINFO, "X composite manager already started.");
        return 1;
    }

    /* Execute xcompmgr */
    pid_t pid = fork();

    switch (pid)
    {
    /* Child */
    case 0:
        execl(ELM_CMD_XCOMPMGR, ELM_CMD_XCOMPMGR, NULL);

        elmprintf(LOGERR, "Error running '%s': %s", ELM_CMD_XCOMPMGR, strerror(errno));
        exit(ELM_EXIT_X_XCOMPMGR);
        break;

    /* Fork failed */
    case -1:
        elmprintf(LOGERR, "Error forking to start '%s': %s.", ELM_CMD_XCOMPMGR,
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
        snprintf(cmd, sizeof(cmd), "%s -merge %s", ELM_CMD_XRDB, xresources);
        elmprintf(LOGINFO, "Executing: %s", cmd);
        system(cmd);
    }

    /* Load .Xmodmap */
    if (access(xmodmap, F_OK) == 0) {
        snprintf(cmd, sizeof(cmd), "%s %s", ELM_CMD_XMODMAP, xmodmap);
        elmprintf(LOGINFO, "Executing: %s", cmd);
        system(cmd);
    }

    return 0;
}

/* ************************************************************************** */
/* Set DISPLAY environment variable */
int elm_x_set_display_env(void)
{
    elmprintf(LOGINFO, "Setting DISPLAY environment variable.");

    if (getenv("DISPLAY")) {
        elmprintf(LOGINFO, "DISPLAY environment variable already set.");
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
        exit(ELM_EXIT_X_DISPLAY);
    }

    /* Set display environment variable */
    char display[3] = {':', i+'0', 0};

    if (elm_setenv("DISPLAY", display) < 0) {
        exit(ELM_EXIT_X_DISPLAY);
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

    if (openpty(&master, &slave, NULL, NULL, NULL) < 0) {
        elmprintf(LOGERR, "Error finding open tty: %s.", strerror(errno));
        exit(ELM_EXIT_X_TTY);
    }

    /* Get tty path from file descriptor */
    char *tty = ttyname(slave);

    if (!tty) {
        elmprintf(LOGERR, "Error finding terminal device path name: %s.",
                  strerror(errno));
        exit(ELM_EXIT_X_TTY);
    }

    /* Set tty environment variable */
    if (elm_setenv("TTY", tty) < 0) {
        exit(ELM_EXIT_X_TTY);
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
    /* ttyn = (strstr(tty, "pts")) ? ttyn+2 : ttyn; */
    if (strstr(tty, "pts"))
        ttyn += 2;

    /* Check tty number */
    if ((ttyn <= 0) || (ttyn > 9)) {
        elmprintf(LOGERR, "Invalid number from tty '%s'.", tty);
        exit(ELM_EXIT_X_TTY);
    }

    /* Set ttyn environment variable */
    char str[2] = {ttyn+'0', 0};

    if (elm_setenv("TTYN", str) < 0) {
        exit(ELM_EXIT_X_TTY);
    }

    return 0;
}

/* ************************************************************************** */
/* Set XAUTHORITY environment variable */
int elm_x_set_xauthority_env(void)
{
    char localhost[HOST_NAME_MAX+1];
    char authfile[64];

    elm_x_get_localhost(localhost, sizeof(localhost)-1);
    elm_x_get_xauth_file(authfile, sizeof(authfile)-1);
    elm_x_set_xauth_entry(authfile, localhost);

    if (elm_setenv("XAUTHORITY", authfile) < 0) {
        exit(100);
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
            elmprintf(LOGINFO, "Set screen Width x Height: '%d x %d'.", ScreenWidth, ScreenHeight);
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
/* Return the VT to use in Xorg command */
int elm_x_get_vt(char *vt, size_t size)
{
    snprintf(vt, size, "vt%s", getenv("TTYN"));

    elmprintf(LOGINFO, "Xorg VT: %s.", vt);

    return 0;
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
    char *home = getenv("HOME");

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
    elm_x_get_random_bytes(&entry.data, entry.data_length);

    /* Open file for writing */
    int   fd;
    FILE *handle;

    elmprintf(LOGINFO, "Opening Xauthority file: '%s'.", filename); 

    if ((fd=open(filename, (O_RDWR | O_CREAT | O_TRUNC), 0700)) < 0) {
        elmprintf(LOGERR, "Unable to open Xauthority file '%s': %s.", filename,
                  strerror(errno));
        free(entry.data);
        return -1;
    }

    elmprintf(LOGINFO, "Opening Xauthority file descriptor: '%d'.", fd); 

    if (!(handle=fdopen(fd, "w+"))) {
        elmprintf(LOGERR, "Unable to open Xauthority file descriptor '%d':%s.",
                  fd, strerror(errno));
        free(entry.data);
        close(fd);
        return -2;
    }

    elmprintf(LOGINFO, "Writing to Xauthority file."); 

    /* Write to xauthority file */
    int status = 0;
    if (!XauWriteAuth(handle, &entry))
        status = 1;

    elmprintf(LOGINFO, "Fflushing writes to Xauthority file."); 

    if (fflush(handle))
        status += 2;

    elmprintf(LOGINFO, "Status: %d", status);

    if (status != 0) {
        elmprintf(LOGERR, "Unable to write to local Xauthority file: '%s'.",
                  filename);
        free(entry.data);
        close(fd);
        fclose(handle);
        return -3;
    }

    /* if (!XauWriteAuth(handle, &entry) || fflush(handle)) { */
    /*     elmprintf(LOGERR, "Unable to write to local Xauthority file: '%s'.", */
    /*               filename); */
    /*     free(entry.data); */
    /*     close(fd); */
    /*     fclose(handle); */
    /*     return -3; */
    /* } */

    elmprintf(LOGINFO, "Writing to wild Xauthority file."); 

    entry.family = FamilyWild;
    if (!XauWriteAuth(handle, &entry) || fflush(handle)) {
        elmprintf(LOGERR, "Unable to write to wild Xauthority file: '%s'.",
                  filename);
        free(entry.data);
        close(fd);
        fclose(handle);
        return -4;
    }

    free(entry.data);
    close(fd);
    fclose(handle);

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
        elmprintf(LOG, "Unable to open '%s' file descriptor: %s.",
                  randfile, strerror(errno));
        return -1;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        elmprintf(LOG, "Unable to seek file descriptor '%d': %s.", fd,
                  strerror(errno));
        return -2;
    }

    /* Copy from urandom */
    *bytes = malloc(size);
    if (read(fd, *bytes, size) < 0) {
        elmprintf(LOG, "Unable to read file descriptor '%d': %s.", fd,
                  strerror(errno));
        return -3;
    }

    close (fd);

    elmprintf(LOGINFO, "Random bytes of length '%lu'.", size);

    return 0;
}

/* ************************************************************************** */
/* Check if X is running */
int elm_x_is_running(void)
{
    return (getenv("DISPLAY")) ? 1 : 0;
}
