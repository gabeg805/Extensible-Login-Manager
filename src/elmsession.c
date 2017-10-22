/* *****************************************************************************
 * 
 * Name:    elmsession.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Create a user session which handles authenticating, logging in,
 *              and logging out.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmsession.h"
#include "elmauthenticate.h"
#include "elmio.h"
#include <stdlib.h>

/* Private functions */
static int elm_session_authenticate(void);
static int elm_session_login(void);
static int elm_session_logout(void);
static int elm_session_alloc(void);

/* Private globals */
static ElmSession *Session = NULL;

/* ************************************************************************** */
/* Create Extensible Login Manager base structure */
ElmSession * elm_session_new(ElmLogin *info)
{
    /* Allocate user session object */
    int status;

    if ((status=elm_session_alloc()) != 0)
        exit(ELM_EXIT_SESSION_NEW);

    Session->authenticate = &elm_session_authenticate;
    Session->login        = &elm_session_login;
    Session->logout       = &elm_session_logout;
    Session->info         = info;

    return Session;
}

/* ************************************************************************** */
/* Authenticate login credentials */
int elm_session_authenticate(void)
{
    if (!Session) {
        elmprintf(LOG, "Unable to authenticate login credentials: Session does not exist.");
        return -1;
    }

    char *username = Session->info->username;
    char *password = Session->info->password;
    elmprintf(LOG, "Authenticating credentials of '%s'." , username);

    return elm_authenticate(username, password);
}

/* ************************************************************************** */
/* Login to user session */
int elm_session_login(void)
{
    if (!Session) {
        elmprintf(LOG, "Unable to login to user session: Session does not exist.");
        return -1;
    }

    char *xsession = Session->info->xsession;
    elmprintf(LOG, "Logging into session '%s'.", xsession);

    return elm_login(xsession, &Session->pid);
}

/* ************************************************************************** */
/* Logout of session */
int elm_session_logout(void)
{
    if (!Session) {
        elmprintf(LOG, "Unable to logout of user session: Session does not exist.");
        return -1;
    }

    char *username = Session->info->username;
    elmprintf(LOG, "Logging out of user session for '%s'.", username);

    return elm_logout();
}

/* ************************************************************************** */
/* Allocate session object */
int elm_session_alloc(void)
{
    Session = calloc(1, sizeof(ElmSession));

    if (!Session) {
        elmprintf(LOG, "Unable to initialize user session: Error allocating memory.");
        return 1;
    }

    return 0;
}
