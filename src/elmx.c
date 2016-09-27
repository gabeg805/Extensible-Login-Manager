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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <X11/Xlib.h>

/* Private functions */
static int xwait(pid_t pid);
static int xtimeout(pid_t pid, uint8_t timeout);
static int xioerror(Display *display);
static int set_display(char *display);
static int set_tty(char *tty);
static int set_ttyn(uint8_t tty);
static char * get_display(void);
static char * get_tty(void);
static uint8_t get_ttyn(char *tty);

/* ************************************************************************** */
/* Start the X server */
/* Note: I added a check if Xorg is already running, but if multiple instances 
 * can be run, it needs to be removed 
 */
int xstartserver(void)
{
    static uint8_t started = 0;
    if ((PREVIEW) || (started))
        return 0;

    elmprintf(LOG, "X server is starting.");
    /* Setup X environment variables */
    char    *display = get_display();
    char    *tty     = get_tty();
    uint8_t  ttyn    = get_ttyn(tty);
    set_display(display);
    set_tty(tty);
    set_ttyn(ttyn);

    /* Start X server */
    const char *xorg = "/usr/bin/Xorg";
    pid_t       pid = fork();
    switch (pid) {
    case 0:
        /* signal(SIGTTIN, SIG_IGN); */
        /* signal(SIGTTOU, SIG_IGN); */
        /* signal(SIGUSR1, SIG_IGN); */
        setpgid(0, getpid());
        const char *authfile = "/tmp/elm.auth";
        const char *logfile  = "/var/log/xserver.log";
        char vt[5];
        snprintf(vt, sizeof(vt), "vt%u", ttyn);
        execl(xorg, xorg, "-logverbose", "-logfile", logfile,
              "-nolisten", "tcp", display, "-auth", authfile, vt, NULL);
        elmprintf(LOG, "Error exec-ing %s: %s.", xorg, strerror(errno));
        exit(ELM_EXIT_X_START);
        break;
    case -1:
        elmprintf(LOG, "Error forking to start %s: %s.", xorg, strerror(errno));
        exit(ELM_EXIT_X_START);
        break;
    default:
        if(xtimeout(pid, 0)) {
            pid = -1;
            return pid;
        }

        /* Wait for server to start up */
        if(xwait(pid)) {
            /* StopServer(); */
            pid = -1;
            exit(ELM_EXIT_X_WAIT);
        }
        break;
    }

    started = 1;
    return 0;
}

/* ************************************************************************** */
/* Start compositing manager (for transparency) */
int xcompmanager(void)
{
    const char *xcompmgr = "/usr/bin/xcompmgr";
    if (pgrep(basename(xcompmgr)))
        return 1;
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
        break;
    }
    return 0;
}

/* ************************************************************************** */
/* Set X server style */
int xstyleserver(void)
{
    exec_proc("/usr/bin/hsetroot -fill /etc/X11/elm/img/night-sky.jpg");
    exec_proc("/usr/bin/xsetroot -cursor_name left_ptr");
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
        elmprintf(LOG, "Exec %s", cmd);
        system(cmd);
    }

    /* Load .Xmodmap */
    char xmodmap[64];
    snprintf(xmodmap, sizeof(xmodmap), "%s/.Xmodmap", home);
    if (access(xmodmap, F_OK) == 0) {
        snprintf(cmd, sizeof(cmd), "/usr/bin/xmodmap %s", xmodmap);
        elmprintf(LOG, "Executing %s", cmd);
        system(cmd);
    }

    return 0;
}
/* ************************************************************************** */
/* Wait for server to start */
int xwait(pid_t pid)
{
    elmprintf(LOG, "Waiting for X server (%u).", pid);
    char    *display  = get_display();
    uint8_t  ntimeout = 30;
    uint8_t  i;

    for (i=0; i < ntimeout; ++i) {
        if (XOpenDisplay(display) != NULL) {
            elmprintf(LOG, "Setting X IO error handler.");
            XSetIOErrorHandler(xioerror);
            return 0;
        } else {
            if (xtimeout(pid, 1)) {
                elmprintf(LOG, "X Timeout (1) has failed.");
                break;
            }
        }
    }
    return 1;
}

/* ************************************************************************** */
/* Wait for server to timeout */
int xtimeout(pid_t pid, uint8_t timeout)
{
    elmprintf(LOG, "X Timeout attempt for pid '%u' with timeout '%u'.", pid, timeout);
    pid_t   pidfound;
    uint8_t i;
    for (i=0; i <= timeout; ++i) {
        pidfound = waitpid(pid, NULL, WNOHANG);
        if (pidfound == pid) {
            elmprintf(LOG, "X Timeout equal pid found '%u'.", pidfound);
            return 1;
        }
        elmprintf(LOG, "X Timeout NON equal pid found '%u'.", pidfound);
        sleep(1);
    }
    return 0;
}

/* ************************************************************************** */
/* Handle X input/output errors */
int xioerror(Display *display)
{
    exit(ELM_EXIT_X_IO);
    /* xrestart(); */
    return 0;
}

/* ************************************************************************** */
/* Restart X server */
/* void xrestart(void) */
/* { */
/*     /\* pam_end(...); *\/ */
/*     StopServer(); */
/*     RemoveLock(); */
/*     while (waitpid(-1, NULL, WNOHANG) > 0); /\* Collects all dead childrens *\/ */
/*     Run(); */
/* } */

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
int set_ttyn(uint8_t tty)
{
    if ((tty == 0) || (tty > 13)) {
        elmprintf(LOG, "Invalid tty number '%u' found.", tty);
        exit(ELM_EXIT_X_TTY);
    }
    char ttystr[3];
    snprintf(ttystr, sizeof(ttystr), "%u", tty);
    if (setenv("TTYN", ttystr, 1) < 0) {
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
    static int  displayval = -1;
    static char displaystr[5];

    /* Find an open display */
    if (displayval < 0) {
        char file[15];
        int  i;
        for (i=0; i < 20; ++i) {
            snprintf(file, sizeof(file), "/tmp/.X%d-lock", i);
            if (access(file, F_OK)) {
                displayval = i;
                break;
            }
        }
        if (displayval < 0) {
            elmprintf(LOG, "Unable to find a free display.");
            exit(ELM_EXIT_X_DISPLAY);
        }
    }

    snprintf(displaystr, sizeof(displaystr), ":%d", displayval);
    return displaystr;
}

/* ************************************************************************** */
/* Return open tty */
char * get_tty(void)
{
    static int  ttyval = -1;
    static char tty[13];

    /* Find open tty */
    if (ttyval < 0) {
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
        size_t shift = strlen("/dev/");
        memset(tty, 0, sizeof(tty));
        snprintf(tty, sizeof(tty), "%s", name+shift);
    }

    return tty;
}

/* ************************************************************************** */
/* Return open tty number */
uint8_t get_ttyn(char *tty)
{
    static uint8_t ttyn = 0;

    /* Find open tty */
    if ((ttyn == 0) || (ttyn > 13)) {
        /* Convert basename of file path to tty number */
        size_t length = strlen(tty);
        size_t shift  = 0;
        while ((!isdigit(tty[shift])) && (shift < length))
            ++shift;
        ttyn = (uint8_t)atoi(tty+shift);
    }

    return ttyn;
}
