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
#include "elmsession.h"
#include "elminterface.h"
#include "elmio.h"
#include "elmx.h"
#include "utility.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>

/* Private functions */
static int elm_login_manager_run(void);
static int elm_login_manager_xinit(void);
static int elm_login_manager_prompt(void);
static void * elm_login_manager_user_session(void*);
static int elm_login_manager_build(void);
static int elm_login_manager_show(void);
static int elm_login_manager_hide(void);
static gboolean elm_login_manager_thread(GtkWidget *, void *);
static int elm_login_manager_alloc(void);
static int elm_login_manager_alloc_apps(size_t);

/* Private globals */
static ElmLoginManager  *Manager = NULL;
static ElmApp          **Apps    = NULL;
static size_t            Size    = 0;
static pthread_t         Thread;

/* ************************************************************************** */
/* Create Extensible Login Manager base structure */
ElmLoginManager * elm_new_login_manager(int argc, char **argv)
{
    /* Allocate login manager object and applications container */
    int status;
    if ((status=elm_login_manager_alloc()) != 0)
        exit(ELM_EXIT_LOGIN_MANAGER_NEW);
    if ((status=elm_login_manager_alloc_apps(1)) < 0)
        exit(ELM_EXIT_LOGIN_MANAGER_APP);

    Manager->run         = &elm_login_manager_run;
    Manager->xinit       = &elm_login_manager_xinit;
    Manager->prompt      = &elm_login_manager_prompt;
    Manager->usersession = &elm_login_manager_user_session;

    /* */
    parse_argv(argc, argv);
    /* */

    return Manager;
}

/* ************************************************************************** */
/* Run login manager */
int elm_login_manager_run(void)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to start Extensible Login Manager (ELM).");
        return ELM_EXIT_LOGIN_MANAGER_RUN;
    }

    elmprintf(LOG, "Starting Extensible Login Manager (ELM).");
    while (true) {
        if (Manager->xinit() != 0)
            return ELM_EXIT_LOGIN_MANAGER_XINIT;
        if (Manager->prompt() != 0)
            return ELM_EXIT_LOGIN_MANAGER_PROMPT;
    }
    return 0;
}

/* ************************************************************************** */
/* Initialize X server */
int elm_login_manager_xinit(void)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to initialize X.");
        return 1;
    }

    xstartserver();
    xcompmanager();
    xstyleserver();

    return 0;
}

/* ************************************************************************** */
/* Display login manager GUI application prompt */
int elm_login_manager_prompt(void)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to display login manager prompt.");
        return 1;
    }

    gtk_init(NULL, NULL);
    elm_login_manager_build();
    elm_login_manager_show();
    gtk_main();

    return 0;
}

/* ************************************************************************** */
/* Run user session */
void * elm_login_manager_user_session(void *arg)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to run user session.");
        return NULL;
    }

    ElmLoginInfo *info    = (ElmLoginInfo*)arg;
    ElmSession   *session = elm_new_session(info);
    int           status;

    /* Authenticate */
    printf("Authenticate\n");
    if ((status=session->authenticate()) != 0) {
        // sleep(3);
        return NULL;
    }
    elmprintf(LOG, "Hiding shit");
    elm_login_manager_hide();

    /* Login */
    printf("Login\n");
    if ((status=session->login()) != 0)
        return NULL;
    elmprintf(LOG, "Session pid: %lu", session->pid);
    waitpid(session->pid, NULL, 0);

    /* Logout */
    printf("Logout\n");
    if ((status=session->logout()) != 0)
        return NULL;
    elmprintf(LOG, "Showing shit");
    elm_login_manager_show();
    Manager->xinit();

    return NULL;
}

/* ************************************************************************** */
/* Build login manager applications */
int elm_login_manager_build(void)
{
    ElmAppBuilder *builder = login_interface();
    size_t         i = 0;
    int            status;
    while (builder[i] != NULL) {
        if ((i+1) >= Size) {
            /* Allocate application */
            if ((status=elm_login_manager_alloc_apps(Size+1)) < 0)
                exit(ELM_EXIT_LOGIN_MANAGER_APP);

            /* Set application to list */
            switch (i) {
            case 3:
                Apps[i] = builder[i](elm_login_manager_thread);
                break;
            default:
                Apps[i] = builder[i](NULL);
                break;
            }
        }
        ++i;
    }
    return 0;
}

/* ************************************************************************** */
/* Show login manager applications */
int elm_login_manager_show(void)
{
    // FIgure out how to design without Size. Do I really even need it?
    size_t i;
    for (i=0; i < Size-1; ++i) {
        Apps[i]->set_self(Apps[i]);
        Apps[i]->show_all();
    }
    return 0;
}

/* ************************************************************************** */
/* Hide login manager applications */
int elm_login_manager_hide(void)
{
    size_t i;
    for (i=0; i < Size-1; ++i) {
        Apps[i]->set_self(Apps[i]);
        Apps[i]->hide_all();
    }
    return 0;
}

/* ************************************************************************** */
/* Thread login manger between GTK and user session */
gboolean elm_login_manager_thread(GtkWidget *widget, void *arg)
{
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to thread login manager.");
        return FALSE;
    }

    pthread_create(&Thread, NULL, Manager->usersession, arg);
    return TRUE;
}

/* ************************************************************************** */
/* Allocate login manager object */
int elm_login_manager_alloc(void)
{
    Manager = (ElmLoginManager*)calloc(1, sizeof(ElmLoginManager));
    if (Manager == NULL) {
        elmprintf(LOG, "Unable to initialize login manager.");
        return 1;
    }

    return 0;
}

/* ************************************************************************** */
/* Allocate application container */
int elm_login_manager_alloc_apps(size_t size)
{
    if (size == 0)
        return 1;

    Apps = (ElmApp**)realloc(Apps, size*sizeof(ElmApp*));
    if (Apps == NULL) {
        elmprintf(LOG, "Unable to allocate application container with size '%u'.",
                  size);
        return -1;
    }
    Apps[size-1] = (ElmApp*)calloc(1, sizeof(ElmApp));
    if (Apps[0] == NULL) {
        elmprintf(LOG, "Unable to allocate application subcontainer.");
        return -1;
    }

    Size = size;
    return 0;
}
