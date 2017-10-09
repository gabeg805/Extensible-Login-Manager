/* *****************************************************************************
 * 
 * Name:    elmx.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Setup the X server and all its components (composite manager, 
 *              background, etc.) for the Extensible Login Manager.
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
#include <Imlib2.h>

/* Private functions */
static void xstop(void);
static void xrestart(void);
static int xopen(void);
static int xblank(void);
static int xtimeout(uint8_t timeout);
static int xwait(void);
static int xignoreio(Display *display);
static int xioerror(Display *display);
static int xsetbackground(void);
static int xsetcursor(void);
static int set_display(char *display);
static int set_tty(char *tty);
static int set_ntty(uint8_t n);
static char * get_display(void);
static char * get_tty(void);
static uint8_t get_ntty(void);

/* Private variables */
static uint8_t  Xstarted   = 0;
static char    *Xdispenv   = NULL;
static pid_t    Xpid       = -1;
static Display *Xdisplay   = NULL;
static Window   Xwindow    = -1;
static Screen  *Xscreen    = NULL;
static int      Xscreennum = -1;
static char    *Xtty       = NULL;
static uint8_t  Xntty      = 0;

jmp_buf xcloseenv;

/* ************************************************************************** */
/* Start the X server */
/* Note: I added a check if Xorg is already running, but if multiple instances 
 * can be run, it needs to be removed 
 */
int xstart(void)
{
    if (Xstarted) {
        elmprintf(LOG, "X server already started.");
        return 0;
    }

    /* Setup X environment variables */
    elmprintf(LOG, "X server is starting.");
    Xdispenv = get_display();
    Xtty     = get_tty();
    Xntty    = get_ntty();
    set_display(Xdispenv);
    set_tty(Xtty);
    set_ntty(Xntty);

    /* Start X server */
    const char *xorg = "/usr/bin/Xorg";
    Xpid = fork();
    switch (Xpid) {
    case 0:
        signal(SIGTTIN, SIG_IGN); /* Slim */
        signal(SIGTTOU, SIG_IGN); /* Slim */
        signal(SIGUSR1, SIG_IGN); /* Slim */
        setpgid(0, getpid()); /* Slim */

        const char *authfile = "/tmp/elm.auth";
        const char *logfile  = "/var/log/xserver.log";
        char vt[5];
        snprintf(vt, sizeof(vt), "vt%02u", Xntty);
        execl(xorg, xorg, "-logverbose", "-logfile", logfile,
              "-keeptty", "-seat", "seat0", "-auth", authfile, Xdispenv, vt, NULL);
        elmprintf(LOG, "Error exec-ing %s: %s.", xorg, strerror(errno));
        exit(ELM_EXIT_X_START);
        break;
    case -1:
        elmprintf(LOG, "Error forking to start %s: %s.", xorg, strerror(errno));
        exit(ELM_EXIT_X_START);
        break;
    default:
        /* sleep(1); */
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

    xopen();
    xblank();
    Xstarted = 1;

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
    xstart();
}

/* ************************************************************************** */
/* Open X window */
int xopen(void)
{
    /* Open display */
    if ((Xdisplay=XOpenDisplay(Xdispenv)) == 0) {
        elmprintf(LOG, "Unabled to open display: %s.", strerror(errno));
        /* xstop(); */
        exit(ELM_EXIT_X_OPEN);
    }

    /* Get screen and root window */
    Xscreennum = DefaultScreen(Xdisplay);
    Xscreen    = DefaultScreenOfDisplay(Xdisplay);
    Xwindow    = RootWindow(Xdisplay, Xscreennum);

    return 0;
}

/* ************************************************************************** */
/* Blank screen */
int xblank(void)
{
	GC gc = XCreateGC(Xdisplay, Xwindow, 0, 0);
	XSetForeground(Xdisplay, gc, BlackPixel(Xdisplay, Xscreennum));
	XFillRectangle(Xdisplay, Xwindow, gc, 0, 0,
				   XWidthOfScreen(ScreenOfDisplay(Xdisplay, Xscreennum)),
				   XHeightOfScreen(ScreenOfDisplay(Xdisplay, Xscreennum)));
	XFlush(Xdisplay);
	XFreeGC(Xdisplay, gc);
    return 0;
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
    uint8_t ntimeout = 120;
    uint8_t i;

    for (i=0; i < ntimeout; ++i) {
        if ((Xdisplay=XOpenDisplay(Xdispenv))) {
            XSetIOErrorHandler(xioerror);
            return 0;
        }
        else
            if (xtimeout(1))
                break;
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
/* Start compositing manager (for transparency) */
int xcompmanager(void)
{
    const char *xcompmgr = "/usr/bin/xcompmgr";
    if (pgrep(basename(xcompmgr))) {
        elmprintf(LOG, "X composite manager already started.");
        return 1;
    }
    pid_t pid = fork();
    switch (pid) {
    case 0:
        execl(xcompmgr, xcompmgr, NULL);
        elmprintf(LOG, "Error exec-ing %s: %s", xcompmgr, strerror(errno));
        exit(ELM_EXIT_X_XCOMPMGR);
        break;
    case -1:
        elmprintf(LOG, "Error forking to start %s: %s.", xcompmgr,
                  strerror(errno));
        return -1;
    default:
        waitpid(pid, NULL, WNOHANG);
        sleep(2);
        break;
    }
    return 0;
}

/* ************************************************************************** */
/* Set X server style */
int xsetstyle(void)
{
    xsetcursor();
    xsetbackground();
    return 0;
}

/* ************************************************************************** */
/* Set background image */
int xsetbackground(void)
{
    /* Load background image */
    const char *filename = "/etc/X11/elm/img/tree.jpg";
    Imlib_Image image    = imlib_load_image(filename);
    if (!image) {
        elmprintf(LOG, "Unable to load image '%s': %s.", filename, strerror(errno));
        return 1;
    }
    imlib_context_set_image(image);

    /* Create X window pixmap */
    elmprintf(LOG, "Creating pixmap.");
    int    width  = imlib_image_get_width();
    int    height = imlib_image_get_height();
    int    depth  = DefaultDepthOfScreen(Xscreen);
    Pixmap pixmap = XCreatePixmap(Xdisplay, Xwindow, width, height, depth);
    switch (pixmap) {
    case BadAlloc:
        elmprintf(LOG, "Unable to create pixmap: Bad allocation: '%s'.", strerror(errno));
        return 1;
    case BadDrawable:
        elmprintf(LOG, "Unable to create pixmap: Bad drawable: '%s'.", strerror(errno));
        return 2;
    case BadValue:
        elmprintf(LOG, "Unable to create pixmap: Bad value: '%s'.", strerror(errno));
        return 3;
    default:
        break;
    }

    /* Prepare setting of background image */
    elmprintf(LOG, "Preparing to set background image.");
    Visual *visual = DefaultVisualOfScreen(Xscreen);
    if (visual == NULL) {
        elmprintf(LOG, "Unable to get visual of screen: %s.", strerror(errno));
        return 5;
    }
    Colormap colormap = DefaultColormapOfScreen(Xscreen);
    imlib_context_set_display(Xdisplay);
    imlib_context_set_visual(visual);
    imlib_context_set_colormap(colormap);
    imlib_context_set_drawable(pixmap);
    imlib_render_image_on_drawable(0, 0);

    /* Set background image */
    elmprintf(LOG, "Setting background pixmap.");
    int status;
    status = XSetWindowBackgroundPixmap(Xdisplay, Xwindow, pixmap);
    switch (status) {
    case BadMatch:
        elmprintf(LOG, "Unable to set background pixmap: Bad match: '%s'.", strerror(errno));
        return 10;
    case BadPixmap:
        elmprintf(LOG, "Unable to set background pixmap: Bad pixmap: '%s'.", strerror(errno));
        return 11;
    case BadWindow:
        elmprintf(LOG, "Unable to set background pixmap: Bad window: '%s'.", strerror(errno));
        return 12;
    default:
        break;
    }

    /* elmprintf(LOG, "Clearing window."); */
    /* status = XClearWindow(Xdisplay, Xwindow); */
    /* switch (status) { */
    /* case BadMatch: */
    /*     elmprintf(LOG, "Unable to clear window: Bad match: '%s'.", strerror(errno)); */
    /*     return 1; */
    /* case BadWindow: */
    /*     elmprintf(LOG, "Unable to clear window: Bad window: '%s'.", strerror(errno)); */
    /*     return 1; */
    /* default: */
    /*     break; */
    /* } */

    /* Flush events to X server */
    XFlush(Xdisplay);
    /* XFreePixmap(Xdisplay, pixmap); */
    /* imlib_free_image(); */
    /* /\* XCloseDisplay(Xdisplay); *\/ */

    return 0;
}

/* ************************************************************************** */
/* Set cursor */
int xsetcursor(void)
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
/* Load xinitrc */
int xinitrc(void)
{
    char *home = getenv("HOME");
    char  cmd[128];

    /* Load .Xresources */
    char xresources[64];
    snprintf(xresources, sizeof(xresources), "%s/.Xresources", home);
    if (access(xresources, F_OK) == 0) {
        snprintf(cmd, sizeof(cmd), "/usr/bin/xrdb -merge %s", xresources);
        elmprintf(LOG, "Executing: %s", cmd);
        system(cmd);
    }

    /* Load .Xmodmap */
    char xmodmap[64];
    snprintf(xmodmap, sizeof(xmodmap), "%s/.Xmodmap", home);
    if (access(xmodmap, F_OK) == 0) {
        snprintf(cmd, sizeof(cmd), "/usr/bin/xmodmap %s", xmodmap);
        elmprintf(LOG, "Executing: %s", cmd);
        system(cmd);
    }

    return 0;
}

/* ************************************************************************** */
/* Set DISPLAY environment variable */
int set_display(char *display)
{
    if (display == NULL)
        return 1;
    if (setenv("DISPLAY", display, 1) < 0) {
        elmprintf(LOG, "Error setting DISPLAY environment variable: %s.",
                  strerror(errno));
        exit(ELM_EXIT_X_DISPLAY);
    }
    return 0;
}

/* ************************************************************************** */
/* Set TTY environment variable */
int set_tty(char *tty)
{
    if (tty == NULL)
        return 1;
    if (setenv("TTY", tty, 1) < 0) {
        elmprintf(LOG, "Error setting TTY environment variable: %s.",
                  strerror(errno));
        exit(ELM_EXIT_X_TTY);
    }
    return 0;
}

/* ************************************************************************** */
/* Set TTYN environment variable */
int set_ntty(uint8_t n)
{
    if (n == 0) {
        elmprintf(LOG, "Invalid tty number '%u' found.", n);
        exit(ELM_EXIT_X_TTY);
    }
    char str[3];
    snprintf(str, sizeof(str), "%u", n);
    if (setenv("TTYN", str, 1) < 0) {
        elmprintf(LOG, "Error setting TTYN environment variable: %s.",
                  strerror(errno));
        exit(ELM_EXIT_X_TTY);
    }
    return 0;
}

/* ************************************************************************** */
/* Return open display */
char * get_display(void)
{
    /* Set global display variable */
    if (Xdispenv == NULL) {
        /* Find an open display */
        int val = -1;
        char file[15];
        int  i;
        for (i=0; i < 20; ++i) {
            snprintf(file, sizeof(file), "/tmp/.X%d-lock", i);
            if (access(file, F_OK)) {
                val = i;
                break;
            }
        }

        /* Set open display */
        if (val < 0) {
            elmprintf(LOG, "Unable to find a free display.");
            exit(ELM_EXIT_X_DISPLAY);
        }

        size_t size = sizeof(int)+2;
        Xdispenv = (char*) calloc(size, sizeof(char));
        snprintf(Xdispenv, size, ":%d", val);
    }

    return Xdispenv;
}

/* ************************************************************************** */
/* Return open tty */
char * get_tty(void)
{
    /* Find open tty */
    if (Xtty == NULL) {
        /* Return tty file descriptor */
        int master, slave;
        if (openpty(&master, &slave, NULL, NULL, NULL) < 0) {
            elmprintf(LOG, "Error finding open tty: %s.", strerror(errno));
            exit(ELM_EXIT_X_TTY);
        }

        /* Convert file descriptor to tty path */
        char *name = ttyname(slave);
        if (name == NULL) {
            elmprintf(LOG, "Error finding terminal device path name: %s.",
                      strerror(errno));
            exit(ELM_EXIT_X_TTY);
        }

        /* Convert basename of file path to tty */
        size_t size = strlen(name)+1;
        Xtty = (char*) calloc(size, sizeof(char));
        snprintf(Xtty, size, "%s", name);


        /* snprintf(tty, sizeof(tty), "/dev/tty7"); */
        /* const char *agetty = "/sbin/agetty"; */
        /* pid_t pid = fork(); */
        /* switch (pid) { */
        /* case 0: */
        /*     elmprintf(LOG, "Executing agetty."); */
        /*     system("/sbin/agetty --noclear tty7 linux"); */
        /*     /\* execl(agetty, agetty, "--noclear", "tty7", "linux", NULL); *\/ */
        /*     elmprintf(LOG, "Done with agetty."); */
        /*     exit(ELM_EXIT_X_START); */
        /*     break; */
        /* case -1: */
        /*     elmprintf(LOG, "Error while forking for tty."); */
        /*     break; */
        /* default: */
        /*     break; */
        /* } */
    }

    return Xtty;
}

/* ************************************************************************** */
/* Return open tty number */
uint8_t get_ntty(void)
{
    /* Check for unset tty */
    if (Xtty == NULL)
        return 0;

    /* Find open tty */
    if (Xntty == 0) {
        /* Convert basename of file path to tty number */
        size_t length = strlen(Xtty);
        size_t shift  = 0;
        while ((!isdigit(Xtty[shift])) && (shift < length))
            ++shift;
        Xntty = (uint8_t)atoi(Xtty+shift);
        if (strstr(Xtty, "pts") != NULL)
            Xntty += 2;
    }

    return Xntty;
}
