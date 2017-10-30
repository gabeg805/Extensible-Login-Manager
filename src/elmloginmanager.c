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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Private functions */
static int    elm_login_manager_run(void);
static int    elm_login_manager_login_prompt(void);
static void * elm_login_manager_login_session(void *data);
static int    elm_login_manager_preview_login(void);
static int    elm_login_manager_build_window(void);
static int    elm_login_manager_build_apps(void);
static int    elm_login_manager_setup_dir(void);
static int    elm_login_manager_setup_xserver(void);
static int    elm_login_manager_setup_signal_catcher(void);
static void   elm_login_manager_signal_catcher(int sig, siginfo_t *info,
                                               void *context);
static int    elm_login_manager_show_apps(void);
static int    elm_login_manager_hide_apps(void);
static void   elm_login_manager_set_preview_mode(int flag);
static void   elm_login_manager_thread(GtkWidget *widget, gpointer data);
static int    elm_login_manager_alloc(void);
static int    elm_login_manager_alloc_apps(size_t s);
static int    elm_login_manager_exists(char *message);

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
    elmprintf(LOGINFO, "Preparing new Login Manager object.");

    if (elm_login_manager_alloc() < 0) {
        exit(ELM_EXIT_MNGR_NEW);
    }

    if (elm_login_manager_alloc_apps(1) < 0) {
        exit(ELM_EXIT_MNGR_APP);
    }

    /* Define methods */
    Manager->run                  = &elm_login_manager_run;
    Manager->login_prompt         = &elm_login_manager_login_prompt;
    Manager->login_session        = &elm_login_manager_login_session;
    Manager->build_window         = &elm_login_manager_build_window;
    Manager->build_apps           = &elm_login_manager_build_apps;
    Manager->setup_dir            = &elm_login_manager_setup_dir;
    Manager->setup_xserver        = &elm_login_manager_setup_xserver;
    Manager->setup_signal_catcher = &elm_login_manager_setup_signal_catcher;
    Manager->show_apps            = &elm_login_manager_show_apps;
    Manager->hide_apps            = &elm_login_manager_hide_apps;
    Manager->set_preview_mode     = &elm_login_manager_set_preview_mode;

    return Manager;
}

/* ************************************************************************** */
/* Run login manager */
int elm_login_manager_run(void)
{
    elmprintf(LOGINFO, "Preparing to run the Extensible Login Manager (ELM).");

    if (!elm_login_manager_exists("run the Extensible Login Manager")) {
        return ELM_EXIT_MNGR_RUN;
    }

    /* Setup */
    if (Manager->setup_dir() < 0) {
        return ELM_EXIT_MNGR_DIR;
    }

    if (Manager->setup_signal_catcher() < 0) {
        return ELM_EXIT_MNGR_SIG_SETUP;
    }

    if (Manager->setup_xserver() < 0) {
        return ELM_EXIT_MNGR_X;
    }

    /* Prompt for username/password */
    while (1) {
        if (Manager->login_prompt() < 0)
            return ELM_EXIT_MNGR_PROMPT;
    }

    return 0;
}

/* ************************************************************************** */
/* Display GUI login manager prompt */
int elm_login_manager_login_prompt(void)
{
    elmprintf(LOGINFO, "Preparing to display login prompt.");

    if (!elm_login_manager_exists("display login prompt")) {
        return -1;
    }

    elmprintf(LOGINFO, "Displaying login prompt.");

    gtk_init(0, 0);

    if (Manager->build_window() < 0) {
        exit(ELM_EXIT_MNGR_BUILD_WIN);
    }

    if (Manager->build_apps() < 0) {
        exit(ELM_EXIT_MNGR_BUILD_APP);
    }

    gtk_main();

    return 0;
}

/* ************************************************************************** */
/* Run login session */
void * elm_login_manager_login_session(void *data)
{
    elmprintf(LOGINFO, "Preparing to run user session.");

    if (!elm_login_manager_exists("run user session")) {
        return NULL;
    }

    ElmLogin   *info    = data;
    ElmSession *session = elm_session_new(info);

    if (!session) {
        exit(ELM_EXIT_SESS_NEW);
    }

    if (session->auth() < 0) {
        /* This does not work, causes SIGSEGV for some reason */
        /* Manager->hide_apps(); */
        /* sleep(2); */
        /* Manager->show_apps(); */
        return NULL;
    }

    if (elm_login_manager_preview_login()) {
        exit(ELM_EXIT_MNGR_PREVIEW);
    }

    Manager->hide_apps();

    if (session->login() < 0) {
        Manager->show_apps();
        return NULL;
    }

    if (session->logout() < 0) {
    }

    Manager->show_apps();

    return NULL;
}

/* ************************************************************************** */
/* Check if login in Preview mode is underway. Exit if it is: login was
 * successful. */
int elm_login_manager_preview_login(void)
{
    if (Preview) {
        elmprintf(LOGINFO, "Login successful in Preview Mode.");
        return 1;
    }

    return 0;
}

/* ************************************************************************** */
/* Build login manager applications */
int elm_login_manager_build_window(void)
{
    elmprintf(LOGINFO, "Building login manager window.");

    elm_x_set_screen_dimensions();
    int width  = elm_x_get_screen_width();
    int height = elm_x_get_screen_height();

    Window    = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    Container = gtk_fixed_new();

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
    elmprintf(LOGINFO, "Building login manager apps.");

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
        elmprintf(LOGINFO, "Adding app '%d' to login manager.", i);

        /* Allocate application */
        if (elm_login_manager_alloc_apps(i+1) < 0) {
            exit(ELM_EXIT_MNGR_APP);
        }

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
/* Setup run directory */
int elm_login_manager_setup_dir(void)
{
    if (access(ELM_RUN_DIR, F_OK)) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to find directory", ELM_RUN_DIR);

        if (mkdir(ELM_RUN_DIR, (S_IRWXU | S_IRWXG | S_IRWXO)) < 0) {
            elmprintf(LOGERRNO, "%s '%s'",
                      "Unable to create directory", ELM_RUN_DIR);
            return -1;
        }
    }

    return 0;
}

/* ************************************************************************** */
/* Setup X server */
int elm_login_manager_setup_xserver(void)
{
    elmprintf(LOGINFO, "Setting up X server.");

    if (!elm_login_manager_exists("setup X server")) {
        return 1;
    }

    if (elm_x_start() < 0) {
        return -1;
    }

    if (elm_x_set_transparency(1) < 0) {
        return 2;
    }

    if (elm_x_set_cursor() < 0) {
    }

    return 0;
}

/* ************************************************************************** */
/* Setup signal catcher */
int elm_login_manager_setup_signal_catcher(void)
{
    elmprintf(LOGINFO, "Setting up signal catcher.");

    if (!elm_login_manager_exists("setup signal catcher")) {
        return -1;
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

    return 0;
}

/* ************************************************************************** */
/* Catch signals */
void elm_login_manager_signal_catcher(int sig, siginfo_t *info, void *context)
{
    if (!elm_login_manager_exists("catch signals")) {
        exit(ELM_EXIT_MNGR_SIG);
    }

    elmprintf(LOG, "Unexpected signal: %d.", sig);
    elmprintf(LOG, "signo:  %d.", info->si_signo);
    elmprintf(LOG, "code:   %d.", info->si_code);
    elmprintf(LOG, "errno:  %d.", info->si_errno);
    elmprintf(LOG, "pid:    %ld.", info->si_pid);
    elmprintf(LOG, "uid:    %ld.", info->si_uid);
    elmprintf(LOG, "status: %d.", info->si_status);

    /* Might not need this */
    int   signo = info->si_signo;
    pid_t pid   = info->si_pid;

    if ((signo == SIGTERM) && (pid == 1)) {
        elmprintf(LOGWARN, "Ignoring signal '%d' from PID '%lu'.", signo, pid);

        struct sigaction act;
        act.sa_flags     = SA_SIGINFO;
        act.sa_sigaction = &elm_login_manager_signal_catcher;
        sigaction(signo, &act, NULL);

        return;
    }

	exit(ELM_EXIT_MNGR_SIG);
}


/* ************************************************************************** */
/* Show widgets */
int elm_login_manager_show_apps(void)
{
    elmprintf(LOGINFO, "Showing login manager.");

    size_t i;
    for (i=0; Widgets[i]; i++) {
        elmprintf(LOGINFO, "Showing widget %d: %p.", i, Widgets[i]);
        gtk_widget_show(Widgets[i]);
    }
    elmprintf(LOGINFO, "Showing Container: %p.", Container);
    gtk_widget_show(Container);
    elmprintf(LOGINFO, "Showing Window: %p.", Window);
    gtk_widget_show(Window);

    return 0;
}

/* ************************************************************************** */
/* Hide widgets */
int elm_login_manager_hide_apps(void)
{
    elmprintf(LOGINFO, "Hiding login manager.");

    size_t i;
    for (i=0; Widgets[i]; i++) {
        elmprintf(LOGINFO, "Hiding widget %d: %p.", i, Widgets[i]);
        gtk_widget_hide(Widgets[i]);
    }
    elmprintf(LOGINFO, "Hiding Container: %p.", Container);
    gtk_widget_hide(Container);
    elmprintf(LOGINFO, "Hiding Window: %p.", Window);
    gtk_widget_hide(Window);

    return 0;
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
    elmprintf(LOGINFO, "Creating login thread.");

    if (!elm_login_manager_exists("create login thread")) {
        exit(ELM_EXIT_MNGR_PTHREAD);
    }

    int status;

    if ((status=pthread_create(&Thread, 0, Manager->login_session, data))) {
        elmprintf(LOGERR, "Error creating thread.");
        exit(ELM_EXIT_MNGR_PTHREAD);
    }
}

/* ************************************************************************** */
/* Allocate login manager object */
int elm_login_manager_alloc(void)
{
    elmprintf(LOGINFO, "Allocating Login Manager object.");

    Manager = calloc(1, sizeof(*Manager));

    if (!Manager) {
        elmprintf(LOGERRNO, "Unable to initialize login manager");
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/* Allocate application container */
int elm_login_manager_alloc_apps(size_t s)
{
    elmprintf(LOGINFO, "Allocating Login Manager applications.");

    size_t size = s+1;
    Widgets     = realloc(Widgets, size * sizeof(*Widgets));

    if (!Widgets) {
        elmprintf(LOGERR, "%s '%lu'",
                  "Unable to allocate widget list with size", size);
        return -1;
    }

    Widgets[size-1] = NULL;

    return 0;
}

/* ************************************************************************** */
/* Check if login manager object exists. Print message if it does not. */
int elm_login_manager_exists(char *message)
{
    if (!Manager) {
        elmprintf(LOGERR, "%s %s: %s.",
                  "Unable to", message, "Login Manager object does not exist");
        return 0;
    }

    return 1;
}
