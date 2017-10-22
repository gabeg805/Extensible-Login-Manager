/* *****************************************************************************
 * 
 * Name:    elmloginmanager.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Control setting up, building, and displaying the ELM.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmloginmanager.h"
#include "elmdef.h"
#include "elminterface.h"
#include "elmio.h"
#include "elmsession.h"
#include "elmx.h"
#include "utility.h"
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#
/* Private functions */
static int    elm_login_manager_run(void);
static int    elm_login_manager_login_prompt(void);
static void * elm_login_manager_login_session(void *data);
static int    elm_login_manager_build_window(void);
static int    elm_login_manager_build_apps(void);
static int    elm_login_manager_show_apps(void);
static int    elm_login_manager_hide_apps(void);
static int    elm_login_manager_setup_x(void);
static int    elm_login_manager_setup_signal_catcher(void);
static void   elm_login_manager_signal_catcher(int sig, siginfo_t *info,
                                               void *context);
static void   elm_login_manager_set_preview_mode(int flag);
static void   elm_login_manager_thread(GtkWidget *widget, gpointer data);
static int    elm_login_manager_alloc(void);
static int    elm_login_manager_alloc_apps(size_t s);

/* Private globals */
static ElmLoginManager  *Manager    = NULL;
static GtkWidget        *Window     = NULL;
static GtkWidget        *Container  = NULL;
static GtkWidget       **Widgets    = NULL;
static pthread_t         Thread;
static int               Preview    = 0;
static const char      * Style      = "/etc/X11/elm/style/css/loginmanager.css";

/* ************************************************************************** */
/* Create Extensible Login Manager base structure */
ElmLoginManager * elm_login_manager_new(void)
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
    Manager->setup_x              = &elm_login_manager_setup_x;
    Manager->setup_signal_catcher = &elm_login_manager_setup_signal_catcher;
    Manager->set_preview_mode     = &elm_login_manager_set_preview_mode;
    Manager->build_window         = &elm_login_manager_build_window;
    Manager->build_apps           = &elm_login_manager_build_apps;
    Manager->show_apps            = &elm_login_manager_show_apps;
    Manager->hide_apps            = &elm_login_manager_hide_apps;

    return Manager;
}

/* ************************************************************************** */
/* Run login manager */
int elm_login_manager_run(void)
{
    elmprintf(LOG, "Starting Extensible Login Manager (ELM).");

    if (!Manager) {
        elmprintf(LOG, "Unable to start Extensible Login Manager (ELM): Login Manager does not exist.");
        return ELM_EXIT_LOGIN_MANAGER_RUN;
    }

    /* Setup */
    if (Manager->setup_signal_catcher() != 0)
        return ELM_EXIT_LOGIN_MANAGER_SETUP_SIGNAL_CATCHER;
    if (Manager->setup_x() != 0)
        return ELM_EXIT_LOGIN_MANAGER_XINIT;

    /* Prompt for username/password */
    while (1) {
        if (Manager->login_prompt() != 0)
            return ELM_EXIT_LOGIN_MANAGER_PROMPT;
    }

    return 0;
}

/* ************************************************************************** */
/* Display GUI login manager prompt */
int elm_login_manager_login_prompt(void)
{
    elmprintf(LOG, "Displaying login prompt.");

    if (!Manager) {
        elmprintf(LOG, "Unable to display login prompt: Login Manager does not exist.");
        return 1;
    }

    gtk_init(0, 0);
    Manager->build_window();
    Manager->build_apps();
    gtk_main();

    return 0;
}

/* ************************************************************************** */
/* Run login session */
void * elm_login_manager_login_session(void *data)
{
    elmprintf(LOG, "Running login session.");

    if (!Manager) {
        elmprintf(LOG, "Unable to run user session: Login Manager does not exist.");
        return NULL;
    }

    ElmLogin   *info    = data;
    ElmSession *session = elm_session_new(info);
    int         status;

    /* Authenticate */
    if ((status=session->authenticate()) != 0) {
        /* This does not work, causes SIGSEGV for some reason */
        /* Manager->hide_apps(); */
        /* sleep(2); */
        /* Manager->show_apps(); */
        return NULL;
    }
    else {
        Manager->hide_apps();

        /* End here during preview mode */
        if (Preview) {
            elmprintf(LOG, "Preview Mode login successful.");
            sleep(2);
            Manager->show_apps();
            return NULL;
        }
    }


    /* Login */
    if ((status=session->login()) != 0) {
        elmprintf(LOG, "Login error.");
        Manager->show_apps();
        return NULL;
    }

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
        elmprintf(LOG, "Logout error.");

    Manager->show_apps();

    return NULL;
}

/* ************************************************************************** */
/* Build login manager applications */
int elm_login_manager_build_window(void)
{
    elmprintf(LOG, "Building login manager window.");

    elm_x_set_screen_dimensions();
    int width  = elm_x_get_screen_width();
    int height = elm_x_get_screen_height();

    Window     = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    Container  = gtk_fixed_new();

    elm_set_widget_style(&Window, "LoginManager", Style);
    gtk_window_set_default_size(GTK_WINDOW(Window), width, height);
    gtk_container_add(GTK_CONTAINER(Window), Container);
    gtk_widget_show(Container);
    gtk_widget_show_all(Window);

    return 0;
}

/* ************************************************************************** */
/* Build login manager applications */
int elm_login_manager_build_apps(void)
{
    elmprintf(LOG, "Building login manager apps.");

    elm_x_set_screen_dimensions();
    int         width  = elm_x_get_screen_width();
    int         height = elm_x_get_screen_height();
    ElmApp     *apps;
    ElmGravity  gravity;
    uint16_t    x;
    uint16_t    y;
    size_t      i;

    for (apps=login_interface(), i=0; apps[i].display; i++)
    {
        elmprintf(LOG, "Adding app '%d' to login manager.", i);

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
    }

    return 0;
}

/* ************************************************************************** */
/* Show widgets */
int elm_login_manager_show_apps(void)
{
    elmprintf(LOG, "Showing login manager.");

    size_t i;
    for (i=0; Widgets[i]; i++)
        gtk_widget_show(Widgets[i]);
    gtk_widget_show(Container);
    gtk_widget_show(Window);

    return 0;
}

/* ************************************************************************** */
/* Hide widgets */
int elm_login_manager_hide_apps(void)
{
    elmprintf(LOG, "Hiding login manager.");

    size_t i;
    for (i=0; Widgets[i]; i++)
        gtk_widget_hide(Widgets[i]);
    gtk_widget_hide(Container);
    gtk_widget_hide(Window);

    return 0;
}

/* ************************************************************************** */
/* Setting up X server */
int elm_login_manager_setup_x(void)
{
    elmprintf(LOG, "Setting up X server.");

    if (!Manager) {
        elmprintf(LOG, "Unable to setup X server: Login Manager does not exist.");
        return 1;
    }

    elm_x_run();
    elm_x_set_transparency(1);
    elm_x_set_cursor();

    return 0;
}

/* ************************************************************************** */
/* Setup signal catcher */
int elm_login_manager_setup_signal_catcher(void)
{
    elmprintf(LOG, "Setting up signal catcher.");

    if (!Manager) {
        elmprintf(LOG, "Unable to setup signal catcher: Login Manager does not exist.");
        return 1;
    }

    struct sigaction act;
    act.sa_flags     = SA_SIGINFO;
    act.sa_sigaction = &elm_login_manager_signal_catcher;

    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGKILL, &act, NULL);
    sigaction(SIGINT,  &act, NULL);
    sigaction(SIGHUP,  &act, NULL);
    sigaction(SIGPIPE, &act, NULL);
    sigaction(SIGUSR1, &act, NULL);

    return 0;
}

/* ************************************************************************** */
/* Catch signals */
void elm_login_manager_signal_catcher(int sig, siginfo_t *info, void *context)
{
    if (!Manager) {
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
    if ((signo == SIGUSR1) || \
        ((signo == SIGTERM) && (pid == 1)))
    {
        elmprintf(LOG, "Ignoring signal '%d' from PID '%lu'.", signo, pid);

        struct sigaction act;
        act.sa_flags     = SA_SIGINFO;
        act.sa_sigaction = &elm_login_manager_signal_catcher;
        sigaction(signo, &act, NULL);

        return;
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
/* Thread login manger between GTK and user session */
void elm_login_manager_thread(GtkWidget *widget, gpointer data)
{
    elmprintf(LOG, "Creating login thread.");

    if (!Manager) {
        elmprintf(LOG, "Unable to create login thread: Login Manager does not exist.");
        return;
    }

    int status;
    if ((status=pthread_create(&Thread, 0, Manager->login_session, data))) {
        elmprintf(LOG, "Error creating thread.");
        return;
    }
}

/* ************************************************************************** */
/* Allocate login manager object */
int elm_login_manager_alloc(void)
{
    Manager = calloc(1, sizeof(*Manager));
    if (!Manager) {
        elmprintf(LOG, "Unable to initialize login manager: Error allocating memory.");
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
