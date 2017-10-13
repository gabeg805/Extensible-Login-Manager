/* *****************************************************************************
 * 
 * Name:    elmloginmanager.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Construct the Extensible Login Manager (C-type) object.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmloginmanager.h"
#include "elmdef.h"
#include "elminterface.h"
#include "elmsession.h"
#include "elmio.h"
#include "elmx.h"
#include "utility.h"
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <systemd/sd-daemon.h>

/* Private functions */
static int      elm_login_manager_run(void);
static void *   elm_login_manager_login_session(void*);
static int      elm_login_manager_login_prompt(void);
static int      elm_login_manager_xinit(void);
static int      elm_login_manager_xstyle(void);
static int      elm_login_manager_setup_signal_catcher(void);
static void     elm_login_manager_signal_catcher(int, siginfo_t *, void *);
static void     elm_login_manager_set_preview_mode(int);
static int      elm_login_manager_build(void);
static int      elm_login_manager_show(void);
static int      elm_login_manager_hide(void);
static gboolean elm_login_manager_thread(GtkWidget *, void *);
static int      elm_login_manager_alloc(void);
static int      elm_login_manager_alloc_apps(size_t);

/* Private globals */
static ElmLoginManager  *Manager    = NULL;
static GtkWidget        *Window     = NULL;
static GtkWidget        *Container  = NULL;
static GtkWidget        *Background = NULL;
static GtkWidget       **Widgets    = NULL;
static pthread_t         Thread;
static int               Preview = 0;


/* ************************************************************************** */
/* Create Extensible Login Manager base structure */
ElmLoginManager * elm_new_login_manager(void)
{
    /* Allocate login manager object and applications container */
    int status;
    if ((status=elm_login_manager_alloc()) != 0)
        exit(ELM_EXIT_LOGIN_MANAGER_NEW);
    if ((status=elm_login_manager_alloc_apps(1)) < 0)
        exit(ELM_EXIT_LOGIN_MANAGER_APP);

    /* Define methods */
    Manager->run                  = &elm_login_manager_run;
    Manager->login_session        = &elm_login_manager_login_session;
    Manager->login_prompt         = &elm_login_manager_login_prompt;
    Manager->xinit                = &elm_login_manager_xinit;
    Manager->xstyle               = &elm_login_manager_xstyle;
    Manager->setup_signal_catcher = &elm_login_manager_setup_signal_catcher;
    Manager->set_preview_mode     = &elm_login_manager_set_preview_mode;

    return Manager;
}

/* ************************************************************************** */
/* Run login manager */
int elm_login_manager_run(void)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to start Extensible Login Manager (ELM): Login Manager does not exist.");
        return ELM_EXIT_LOGIN_MANAGER_RUN;
    }

    elmprintf(LOG, "Starting Extensible Login Manager (ELM).");
    if (Manager->setup_signal_catcher() != 0)
        return ELM_EXIT_LOGIN_MANAGER_SETUP_SIGNAL_CATCHER;
    if (Manager->xinit() != 0)
        return ELM_EXIT_LOGIN_MANAGER_XINIT;

    while (1) {
        elmprintf(LOG, "Iterating through run loop.");
        if (Manager->xstyle() != 0)
            return ELM_EXIT_LOGIN_MANAGER_XSTYLE;
        if (Manager->login_prompt() != 0)
            return ELM_EXIT_LOGIN_MANAGER_PROMPT;
    }

    return 0;
}

/* ************************************************************************** */
/* Run login session */
void * elm_login_manager_login_session(void *arg)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to run user session: Login Manager does not exist.");
        return NULL;
    }

    elmprintf(LOG, "Running login session.");
    ElmLogin   *info    = arg;
    ElmSession *session = elm_new_session(info);
    int         status;

    /* Authenticate */
    if ((status=session->authenticate()) != 0)
        return NULL;
    else {
        elm_login_manager_hide();
        sleep(2);
    }

    /* End here during preview mode */
    if (Preview) {
        elmprintf(LOG, "Login successful.");
        elm_login_manager_show();
        return NULL;
    }

    /* Login */
    if ((status=session->login()) != 0)
        return NULL;

    /* Wait for session to end */
    int wstatus;
    if ((status=waitpid(session->pid, &wstatus, 0)) == -1)
        elmprintf(LOG, "Waitpid errored: %s.", strerror(errno));

    if (WIFEXITED(wstatus))
        elmprintf(LOG, "Exited with status '%d'.", WEXITSTATUS(wstatus));
    else if (WIFSIGNALED(wstatus))
        elmprintf(LOG, "Killed by signal '%d'.", WTERMSIG(wstatus));
    else if (WIFSTOPPED(wstatus))
        elmprintf(LOG, "Stopped by signal '%d'.", WSTOPSIG(wstatus));
    else if (WIFCONTINUED(wstatus))
        elmprintf(LOG, "Resumed by delivery of SIGCONT.");
    else
        ;

    /* Logout */
    if ((status=session->logout()) != 0)
        return NULL;
    elm_login_manager_show();

    return NULL;
}

/* ************************************************************************** */
/* Display GUI login manager prompt */
int elm_login_manager_login_prompt(void)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to display login prompt: Login Manager does not exist.");
        return 1;
    }

    elmprintf(LOG, "Displaying login prompt.");
    gtk_init(NULL, NULL);
    elm_login_manager_build();
    elm_login_manager_show();
    sd_notify(0, "READY=1");
    gtk_main();

    return 0;
}

/* ************************************************************************** */
/* Initialize X server */
int elm_login_manager_xinit(void)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to initialize X: Login Manager does not exist.");
        return 1;
    }

    /* Can maybe move this down after Xdisplay is set? */
    elmprintf(LOG, "Initializing X server.");
    if (Preview)
        elmprintf(LOG, "Skipping X server 'start' due to Preview Mode.");
    else
        xstart();
    xcompmanager();

    return 0;
}

/* ************************************************************************** */
/* Style X server */
int elm_login_manager_xstyle(void)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to style X: Login Manager does not exist.");
        return 1;
    }

    elmprintf(LOG, "Setting X server style.");
    if (Preview)
        elmprintf(LOG, "Skipping X server 'setstyle' due to Preview Mode (To-Do: should not skip this).");
    else
        xsetstyle();

    return 0;
}

/* ************************************************************************** */
/* Setup signal catcher */
int elm_login_manager_setup_signal_catcher(void)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to setup signal catcher: Login Manager does not exist.");
        return 1;
    }

    elmprintf(LOG, "Setting up signal catcher.");
    struct sigaction act;
    act.sa_flags     = SA_SIGINFO;
    act.sa_sigaction = &elm_login_manager_signal_catcher;
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGKILL, &act, NULL);
    sigaction(SIGINT,  &act, NULL);
    sigaction(SIGHUP,  &act, NULL);
    sigaction(SIGPIPE, &act, NULL);

    return 0;
}

/* ************************************************************************** */
/* Catch signals */
void elm_login_manager_signal_catcher(int sig, siginfo_t *info, void *context)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to catch signals: Login Manager does not exist.");
        exit(ELM_EXIT_LOGIN_MANAGER_SIGNAL_CATCHER);
    }

    elmprintf(LOG, "Unexpected signal: %d.", sig);
    elmprintf(LOG, "signo:  %d.", info->si_signo);
    elmprintf(LOG, "code:   %d.", info->si_code);
    elmprintf(LOG, "errno:  %d.", info->si_errno);
    elmprintf(LOG, "pid:    %ld.", info->si_pid);
    elmprintf(LOG, "uid:    %ld.", info->si_uid);
    elmprintf(LOG, "status: %d.", info->si_status);

    int   signo = info->si_signo;
    pid_t pid   = info->si_pid;
    if (signo == SIGTERM) {
        if (pid == 1) {
            elmprintf(LOG, "Ignoring SIGTERM from PID '%lu'.", pid);
            struct sigaction act;
            act.sa_flags     = SA_SIGINFO;
            act.sa_sigaction = &elm_login_manager_signal_catcher;
            sigaction(signo, &act, NULL);
            return;
        }
    }

	exit(ELM_EXIT_LOGIN_MANAGER_SIGNAL_CATCHER);
}

/* ************************************************************************** */
/* Set preview mode flag */
void elm_login_manager_set_preview_mode(int flag)
{
    Preview = flag;
}

/* ************************************************************************** */
/* Build login manager applications */
int elm_login_manager_build(void)
{
    int width;
    int height;
    get_display_dimensions(&width, &height);
    printf("Width: %d | Height: %d\n", width, height);

    Window     = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    Container  = gtk_fixed_new();
    Background = gtk_image_new_from_file("/etc/X11/elm/img/tree.jpg");

    gtk_window_set_default_size(GTK_WINDOW(Window), width, height);
    gtk_container_add(GTK_CONTAINER(Window), Container);
    gtk_fixed_put(GTK_FIXED(Container), Background, 0, 0);

    ElmApp     *apps = login_interface();
    size_t      i    = 0;
    ElmGravity  gravity;
    uint16_t    x;
    uint16_t    y;

    while (apps[i].display)
    {
        /* Allocate application */
        if (elm_login_manager_alloc_apps(i+1) < 0)
            exit(ELM_EXIT_LOGIN_MANAGER_APP);

        /* Append widget to list */
        Widgets[i] = apps[i].display(elm_login_manager_thread);

        /* Add widget to container */
        gravity = apps[i].gravity;
        x       = apps[i].x;
        y       = apps[i].y;

        switch (gravity) {
        case ELM_GRAV_TOP_LEFT:
            break;
        case ELM_GRAV_TOP_RIGHT:
            x = width - x;
            break;
        case ELM_GRAV_CENTER:
            x = width/2  + x;
            y = height/2 + y;
            break;
        case ELM_GRAV_BOTTOM_LEFT:
            y = height - y;
            break;
        case ELM_GRAV_BOTTOM_RIGHT:
            x = width  - x;
            y = height - y;
            break;
        default:
            break;
        }

        gtk_fixed_put(GTK_FIXED(Container), Widgets[i], x, y);

        i++;
    }

    gtk_widget_show(Background);
    gtk_widget_show(Container);
    gtk_widget_show(Window);

    return 0;
}

/* ************************************************************************** */
/* Show login manager applications */
int elm_login_manager_show(void)
{
    size_t i;
    for (i=0; Widgets[i]; i++)
        gtk_widget_show_all(Widgets[i]);

    return 0;
}

/* ************************************************************************** */
/* Hide login manager applications */
int elm_login_manager_hide(void)
{
    size_t i;
    for (i=0; Widgets[i]; i++)
        gtk_widget_hide(Widgets[i]);

    return 0;
}

/* ************************************************************************** */
/* Thread login manger between GTK and user session */
gboolean elm_login_manager_thread(GtkWidget *widget, void *arg)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to thread login manager: Login Manager does not exist.");
        return FALSE;
    }

    pthread_create(&Thread, NULL, Manager->login_session, arg);

    return TRUE;
}

/* ************************************************************************** */
/* Allocate login manager object */
int elm_login_manager_alloc(void)
{
    Manager = calloc(1, sizeof(*Manager));
    if (!Manager) {
        elmprintf(LOG, "Unable to initialize login manager: Login Manager does not exist.");
        return 1;
    }

    return 0;
}

/* ************************************************************************** */
/* Allocate application container */
int elm_login_manager_alloc_apps(size_t s)
{
    size_t size = s+1;

    Widgets = realloc(Widgets, size*sizeof(*Widgets));
    if (!Widgets) {
        elmprintf(LOG, "Unable to allocate widget list with size '%lu'.",
                  size);
        return -1;
    }

    Widgets[size-1] = NULL;

    return 0;
}
