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
#include "elmconf.h"
#include "elmdef.h"
#include "elmio.h"
#include "elmsys.h"
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <pty.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* #include <dbus/dbus.h> */
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xauth.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>

/* Do i need this */
#include <ctype.h>

/* Private functions */
static int    elm_x_wait(void);
static int    elm_x_init(void);
static int    elm_x_stop(Display *display);
static int    elm_x_exec_xorg(void);
static int    elm_x_exec_xcompmgr(void);
static int    elm_x_set_env(void);
static int    elm_x_set_display_env(void);
static int    elm_x_set_tty_env(void);
static int    elm_x_set_ttyn_env(void);
static int    elm_x_set_xauthority_env(void);
static int    elm_x_set_xorgvt_env(void);
static int    elm_x_set_xauth_entry(char *filename, char *localhost);
static char * elm_x_get_xauth_file(void);
static char * elm_x_get_localhost(void);
static char * elm_x_get_vt(void);
static char * elm_x_get_random_bytes(size_t size);
static char * elm_x_get_tty(void);
static char * elm_x_get_tty_from_pts(void);
static char * elm_x_get_tty_from_proc(void);
static char * elm_x_get_tty_from_sys(void);
static int    elm_x_is_running(void);

/* Private variables */
static Display *XDisplay = NULL;
static pid_t    XPid     = -1;

/* ************************************************************************** */
/* Start the X server */
/* Note: I added a check if Xorg is already running, but if multiple instances 
 * can be run, it needs to be removed 
 */
int elm_x_start(void)
{
    elmprintf(LOGINFO, "Starting X server."); 

    if (elm_x_is_running()) {
        elmprintf(LOGINFO, "X server already running.");
    }
    else {
        if (elm_x_set_env() < 0) {
            return -2;
        }

        if (elm_x_exec_xorg() < 0) {
            return -3;
        }
    }

    if (elm_x_init() < 0) {
        exit(ELM_EXIT_X_INIT);
    }

    /* DBusError derr; */
    /* dbus_err_init(&derr); */
    /* DBusConnection *dcon = dbus_bus_get(DBUS_BUS_SYSTEM */

    return 0;
}

/* ************************************************************************** */
/* Wait for SIGUSR1 from Xorg, indicating it started successfully. Timeout in 30
 * seconds */
int elm_x_wait(void)
{
    elmprintf(LOGINFO, "Setting up wait for 'SIGUSR1' signal.");

    struct timespec timeout = {0};
    sigset_t        set;
    siginfo_t       info;
    int             sec;

    if ((sec=elm_conf_read_int("Main", "XTimeout")) < 0) {
        sec = 30;
    }

    timeout.tv_sec = sec;

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

    if (!(XDisplay=XOpenDisplay(display))) {
        elmprintf(LOGERRNO, "%s '%s'", "Unable to open display on", display);
        elm_x_stop(XDisplay);
        return -1;
    }

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
    if(XDisplay) {
        XCloseDisplay(XDisplay);
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

    if(XPid < 0) {
        elmprintf(LOGERR, "X server has invalid pid '%lu'. Exiting.", XPid);
    }
    else {
        errno = 0;
        if(killpg(XPid, SIGTERM) < 0) {
            elmprintf(LOGERRNO, "Unable to terminate X server process group");
            exit(ELM_EXIT_X_STOP);
        }
    }

    /* Wait 10 sec for server to shut down */
    elmprintf(LOGWARN, "Waiting for X server to shut down.");

    int i;
    for (i=0; i < 10; i++)
    {
        if (waitpid(XPid, 0, WNOHANG) == XPid) {
            elmprintf(LOGWARN, "X server shutting down.");
            exit(ELM_EXIT_X_STOP);
        }

        sleep(1);
    }

    /* Send KILL to server */
    elmprintf(LOGWARN, "%s. %s", "X server is slow to shut down.",
              "Sending KILL to X server process group.");

    errno = 0;
    if(killpg(XPid, SIGKILL) < 0) {
        elmprintf(LOGERRNO, "Unable to KILL X server process group");
    }

    /* Collect all dead children */
    elmprintf(LOGWARN, "Collecting all dead children.");

    while (waitpid(-1, 0, WNOHANG) > 0);

    exit(ELM_EXIT_X_STOP);
}

/* ************************************************************************** */
/* Execute X command */
int elm_x_exec_xorg(void)
{
    char *display    = getenv("DISPLAY");
    char *xauthority = getenv("XAUTHORITY");
    char *vt         = getenv("XORGVT");
    char *argv[]     = {ELM_CMD_XORG, display, "-background", "none",
                        "-noreset", "-verbose", "3", "-logverbose", "-logfile",
                        ELM_XLOG, "-auth", xauthority, "-seat", "seat0",
                        "-nolisten", "tcp", vt, NULL};

    /* Start X server */
    switch ((XPid=fork()))
    {
    case 0:
        elmprintf(LOGINFO, "Preparing to run X server.");

        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGUSR1, SIG_IGN);
        setpgid(0, getpid());

        elm_sys_exec(argv[0], argv);
        exit(ELM_EXIT_X_EXEC);
    case -1:
        elmprintf(LOGERRNO, "%s '%s'", "Error during fork to start", argv[0]);
        exit(ELM_EXIT_X_EXEC);
    default:
        if (elm_x_wait() < 0) {
            exit(ELM_EXIT_X_WAIT);
        }
        break;
    }

    return 0;
}

/* ************************************************************************** */
/* Execute xcompmgr command */
int elm_x_exec_xcompmgr(void)
{
    char  *argv[] = {ELM_CMD_XCOMPMGR, NULL};
    pid_t  pid;

    switch ((pid=fork()))
    {
    case 0:
        elm_sys_exec(argv[0], argv);
        return 1;
    case -1:
        elmprintf(LOGERRNO, "%s '%s'", "Error during fork to start", argv[0]);
        return 2;
    default:
        waitpid(pid, 0, WNOHANG);
        break;
    }

    return 0;
}

/* ************************************************************************** */
/* Set cursor */
int elm_x_set_cursor(void)
{
    elmprintf(LOGINFO, "Preparing to set cursor.");

    Cursor cursor = XCreateFontCursor(XDisplay, XC_left_ptr);

    switch (cursor)
    {
    case BadAlloc:
        elmprintf(LOGERRNO, "Unable to create font cursor: Bad allocation");
        return -1;
    case BadValue:
        elmprintf(LOGERRNO, "Unable to create font cursor: Bad value");
        return -2;
    default:
        break;
    }

    Window window = DefaultRootWindow(XDisplay);
    int    status = XDefineCursor(XDisplay, window, cursor);

    switch (status)
    {
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
    XFlush(XDisplay);

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
        return 1;
    }

    /* Check if transparency program is running */
    char *basename = elm_sys_basename(ELM_CMD_XCOMPMGR);

    if (elm_sys_pgrep(basename)) {
        elmprintf(LOGINFO, "X composite manager already started.");
        return 2;
    }

    return elm_x_exec_xcompmgr();
}

/* ************************************************************************** */
/* Load preferences from Xresources and Xmodmap */
int elm_x_load_user_preferences(void)
{
    char *home = getenv("HOME");
    char  xresources[ELM_MAX_CRED_SIZE];
    char  xmodmap[ELM_MAX_CRED_SIZE];
    char  cmd[ELM_MAX_CMD_SIZE];

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
        elmprintf(LOGWARN, "%s: '%s=%s'.",
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
    char *tty    = elm_x_get_tty();
    int   status = 0;

    if (elm_setenv("TTY", tty) < 0) {
        status = -1;
    }

    free(tty);

    return status;
}

/* ************************************************************************** */
/* Set TTYN environment variable */
int elm_x_set_ttyn_env(void)
{
    char   *tty    = getenv("TTY");
    size_t  length = strlen(tty);
    size_t  shift  = 0;
    int     ttyn   = 0;

    /* Get tty number from name in path */
    while ((!isdigit(tty[shift])) && (shift < length)) {
        ++shift;
    }

    ttyn = atoi(tty+shift);
    ttyn = (strstr(tty, "pts")) ? ttyn+2 : ttyn;

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
    char *localhost  = NULL;
    char *xauthority = NULL;
    int   status     = 0;

    if (!(localhost=elm_x_get_localhost())) {
        status = -1;
        goto cleanup;
    }


    if (!(xauthority=elm_x_get_xauth_file())) {
        status = -2;
        goto cleanup;
    }

    if (elm_x_set_xauth_entry(xauthority, localhost) < 0) {
        status = -3;
        goto cleanup;
    }

    if (elm_setenv("XAUTHORITY", xauthority) < 0) {
        status = -4;
        goto cleanup;
    }

cleanup:
    free(localhost);
    free(xauthority);

    localhost  = NULL;
    xauthority = NULL;

    return status;
}

/* ************************************************************************** */
/* Set XORGVT environment variable. Note: This is not a standard environment
 * variable but I am setting it to keep the X run function consistent.
 */
int  elm_x_set_xorgvt_env(void)
{
    char *vt     = NULL;
    int   status = 0;

    if (!(vt=elm_x_get_vt())) {
        status = -1;
        goto cleanup;
    }

    if (elm_setenv("XORGVT", vt) < 0) {
        status = -2;
        goto cleanup;
    }

cleanup:
    free(vt);

    vt = NULL;

    return status;
}

/* ************************************************************************** */
/* Return the Xauthority entry */
int elm_x_set_xauth_entry(char *filename, char *localhost)
{
    FILE  *handle = NULL;
    Xauth  entry  = {0};
    int    fd     = -1;
    int    status = 0;

    entry.family         = FamilyLocal;
    entry.address        = localhost;
    entry.address_length = strlen(entry.address);
    entry.name           = "MIT-MAGIC-COOKIE-1";
    entry.name_length    = strlen(entry.name);
    entry.data_length    = 16;
    entry.data           = elm_x_get_random_bytes(entry.data_length);

    if (!(entry.data)) {
        status = -1;
        goto cleanup;
    }

    /* Open file for writing */
    if ((fd=open(filename, (O_RDWR | O_CREAT | O_TRUNC), 0700)) < 0) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to open Xauthority file", filename);
        status = -2;
        goto cleanup;
    }

    if (!(handle=fdopen(fd, "w+"))) {
        elmprintf(LOGERR, "%s '%d'",
                  "Unable to open Xauthority file descriptor", fd);
        status = -3;
        goto cleanup;
    }

    /* Write to xauthority file */
    if (!XauWriteAuth(handle, &entry) || fflush(handle)) {
        elmprintf(LOGERR, "%s: '%s'.",
                  "Unable to write to local Xauthority file", filename);
        status = -4;
        goto cleanup;
    }

    entry.family = FamilyWild;

    if (!XauWriteAuth(handle, &entry) || fflush(handle)) {
        elmprintf(LOGERR, "%s: '%s'.",
                  "Unable to write to wild Xauthority file", filename);
        status = -5;
        goto cleanup;
    }

cleanup:
    free(entry.data);
    close(fd);
    fclose(handle);

    return status;
}

/* ************************************************************************** */
/* Set screen width and height. These are the dimensions of the active
 * monitor.
 */
int elm_x_screen_dimensions(int *width, int *height)
{
    Window              window = DefaultRootWindow(XDisplay);
    XRRScreenResources *screen = XRRGetScreenResources(XDisplay, window);

    /* Unable to determine screen info */
    if (!screen) {
        elmprintf(LOGERR, "Unable to set screen dimensions.");
        return 1;
    }

    /* Check if width and height already set in config file */
    *width  = elm_conf_read_int("Main", "ScreenWidth");
    *height = elm_conf_read_int("Main", "ScreenHeight");

    if ((*width > 0) && (*height > 0)) {
        return 0;
    }

    /* Width and height were not set. Set to initial values */
    if (*width < 0) {
        *width = 0;
    }

    if (*height < 0) {
        *height = 0;
    }

    /* Set screen dimensions for the monitor at x=0 */
    int          num = screen->ncrtc;
    int          i;
    XRRCrtcInfo *info;

    for (i=0; i < num; i++) {
        info = XRRGetCrtcInfo(XDisplay, screen, screen->crtcs[i]);

        if (info->x == 0) {
            *width  = info->width;
            *height = info->height;
            XRRFreeCrtcInfo(info);
            break;
        }

        XRRFreeCrtcInfo(info);
    }

    XRRFreeScreenResources(screen);

    return 0;
}

/* ************************************************************************** */
/* Return the Xauthority file path */
char * elm_x_get_xauth_file(void)
{
    /* Store file path in temp array */
    char file[ELM_MAX_PATH_SIZE];
 
    if (access(ELM_RUN_DIR, W_OK) == 0) {
        snprintf(file, sizeof(file), "%s/.Xauthority", ELM_RUN_DIR);
    }
    else {
        snprintf(file, sizeof(file), "/tmp/.Xauthority");
    }

    /* Allocate memory for path and copy string */
    size_t  size       = strlen(file)+1;
    char   *xauthority = calloc(size, sizeof(char));
    strncpy(xauthority, file, size);

    elmprintf(LOGINFO, "X authority file: '%s'.", xauthority);

    return xauthority;
}

/* ************************************************************************** */
/* Return the localhost for the Xauthority entry */
char * elm_x_get_localhost(void)
{
    size_t  size      = HOST_NAME_MAX;
    char   *localhost = calloc(size, sizeof(char));

    if (gethostname(localhost, size) < 0) {
        elmprintf(LOGERR, "Unable to 'gethostname'.");
        strncpy(localhost, "localhost", size);
    }

    elmprintf(LOGINFO, "X authority localhost: %s.", localhost); 

    return localhost;
}

/* ************************************************************************** */
/* Return the VT to use in Xorg command */
char * elm_x_get_vt(void)
{
    size_t  size = 5;
    char   *vt   = calloc(size, sizeof(char));

    snprintf(vt, size, "vt%s", getenv("TTYN"));

    elmprintf(LOGINFO, "Xorg VT: %s.", vt);

    return vt;
}

/* ************************************************************************** */
/* Return random number of bytes */
char * elm_x_get_random_bytes(size_t size)
{
    char randfile[] = "/dev/urandom";
    int  fd         = open(randfile, O_RDONLY);

    /* Check urandom file */
    if (fd < 0) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to open file descriptor for", randfile);
        return NULL;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        elmprintf(LOGERRNO,
                  "Unable to seek file descriptor to read random bytes");
        return NULL;
    }

    /* Copy from urandom */
    char *bytes = calloc(size+1, sizeof(char));

    if (!bytes) {
        elmprintf(LOGERRNO, "Unable to allocate memory to read random bytes.");
        return NULL;
    }

    if (read(fd, bytes, size) < 0) {
        elmprintf(LOGERRNO, "Unable to read file descriptor to read random bytes");
        return NULL;
    }

    close (fd);

    return bytes;
}

/* ************************************************************************** */
/* Return open tty */
/* Note: stackoverflow.com/questions/12181820/get-foreground-console-find-active-x-server */
char * elm_x_get_tty(void)
{
    char *tty;

    if ((tty=elm_x_get_tty_from_proc())) {
        return tty;
    }

    if ((tty=elm_x_get_tty_from_pts())) {
        return tty;
    }

    if ((tty=elm_x_get_tty_from_sys())) {
        return tty;
    }

    return NULL;
}

/* ************************************************************************** */
/* Return open pts acting as a tty */
char * elm_x_get_tty_from_pts(void)
{
    char   name[ELM_MAX_CMD_SIZE] = {0};
    char  *tty;
    char  *ptr;
    int    master;
    int    slave;
    size_t length;

    /* Find open pseudoterminal */
    if (openpty(&master, &slave, name, NULL, NULL) < 0) {
        elmprintf(LOGERRNO, "Error finding open pseudoterminal");
        return NULL;
    }

    /* Return pseudoterminal */
    ptr    = (strstr(name, "/dev/")) ? &name[5] : name;
    length = strlen(ptr)+1;
    tty    = calloc(length, sizeof(*tty));

    strncpy(tty, ptr, length);

    return tty;
}

/* ************************************************************************** */
/* Return open tty by searching /proc directory */
char * elm_x_get_tty_from_proc(void)
{
    int            ttyignore[7] = {0};
    char          *proc;
    char           dirpath[ELM_MAX_PATH_SIZE];
    char           cmdpath[ELM_MAX_PATH_SIZE];
    char           symlinkpath[ELM_MAX_PATH_SIZE];
    char           fullpath[ELM_MAX_PATH_SIZE];
    DIR           *dstream;
    struct dirent *entry;

    /* Iterate over current process ids */
    while ((proc=elm_sys_get_proc()))
    {
        snprintf(dirpath, sizeof(dirpath), "/proc/%s/fd/", proc);

        /* Unable to open directory */
        if (!(dstream=opendir(dirpath))) {
            elmprintf(LOGERRNO, "%s '%s'",
                      "Unable to open directory", dirpath);
            continue;
        }

        /* Iterate over directory contents */
        while ((entry=readdir(dstream))) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }

            /* Path too long */
            size_t dlen = strlen(dirpath);
            size_t nlen = strlen(entry->d_name);

            symlinkpath[0] = 0;

            if ((dlen+nlen+1) >= sizeof(symlinkpath)) {
                continue;
            }

            /* Determine full path */
            strncat(symlinkpath, dirpath, dlen);
            strncat(symlinkpath, entry->d_name, nlen);
            memset(fullpath, 0, sizeof(fullpath));
            readlink(symlinkpath, fullpath, sizeof(fullpath));

            /* printf("\t\t'%s'\n", symlinkpath); */

            /* /\* Check if should ignore this process *\/ */
            /* if (strstr(fullpath, "/sys/devices/virtual/tty")) { */
            /*     break; */
            /* } */

            /* Check if tty is in name */
            if (strstr(fullpath, "/dev/tty")) {
                snprintf(cmdpath, sizeof(cmdpath), "/proc/%s/cmdline", proc);

                char *tty = elm_sys_read_line(cmdpath);
                int   num = fullpath[8]-'0';

                if (!tty || ttyignore[num-1]) {
                    free(tty);
                    continue;
                }

                if (strstr(tty, "Xorg")) {
                    ttyignore[num-1] = 1;
                }

                free(tty);
            }
        }

        closedir(dstream);
    }

    /* Return open tty */
    char   *opentty = NULL;
    size_t  length  = 5;
    int     i;

    for (i=0; i < sizeof(ttyignore); i++) {
        if (!ttyignore[i]) {
            opentty = calloc(length, sizeof(*opentty));
            snprintf(opentty, length, "tty%d", i+1);
            break;
        }
    }

    if (!opentty) {
        elmprintf(LOGERR, "%s",
                  "Unable to find open tty by searching '/proc' directory.");
    }

    return opentty;
}

/* ************************************************************************** */
/* Return active tty from /sys/ directory */
char * elm_x_get_tty_from_sys(void)
{
    char *activefile              = "/sys/devices/virtual/tty/tty0/active";

    return elm_sys_read_line(activefile);
}

/* ************************************************************************** */
/* Check if X is running */
int elm_x_is_running(void)
{
    return (getenv("DISPLAY")) ? 1 : 0;
}
