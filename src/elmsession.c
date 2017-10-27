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
#include "elmdef.h"
#include "elmio.h"
#include "elmpam.h"
#include <stdlib.h>

/* Private functions */
static int elm_session_auth(void);
static int elm_session_login(void);
static int elm_session_logout(void);
static int elm_session_alloc(void);
static int elm_session_exists(char *message);

/* Private globals */
static ElmSession *Session = NULL;

/* ************************************************************************** */
/* Create Extensible Login Manager base structure */
ElmSession * elm_session_new(ElmLogin *info)
{
    elmprintf(LOGINFO, "Preparing new Session object.");

    if (elm_session_alloc() < 0) {
        exit(ELM_EXIT_SESS_NEW);
    }

    Session->auth   = &elm_session_auth;
    Session->login  = &elm_session_login;
    Session->logout = &elm_session_logout;
    Session->info   = info;

    return Session;
}

/* ************************************************************************** */
/* Authenticate login credentials */
int elm_session_auth(void)
{
    elmprintf(LOGINFO, "Preparing to authenticate login credentials.");

    if (!elm_session_exists("authenticate login credentials")) {
        return -1;
    }

    if (elm_pam_init(Session->info) < 0) {
        return -2;
    }

    if (elm_pam_auth() < 0) {
        return -3;
    }

    return 0;
}

/* ************************************************************************** */
/* Login to user session */
int elm_session_login(void)
{
    elmprintf(LOGINFO, "Preparing to login to user session.");

    if (!elm_session_exists("login to user session")) {
        return -1;
    }

    if (elm_pam_login() < 0) {
        return -2;
    }

    return 0;
}

/* ************************************************************************** */
/* Logout of session */
int elm_session_logout(void)
{
    elmprintf(LOGINFO, "Preparing to logout of user session");

    if (!elm_session_exists("logout of user session")) {
        return -1;
    }

    if (elm_pam_logout() < 0) {
        return -2;
    }

    return 0;
}

/* ************************************************************************** */
/* Allocate session object */
int elm_session_alloc(void)
{
    elmprintf(LOGINFO, "Allocating Session object.");

    Session = calloc(1, sizeof(ElmSession));

    if (!Session) {
        elmprintf(LOGERRNO, "Unable to initialize user session");
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/* Check if session object exists. Print message if it does not. */
int elm_session_exists(char *message)
{
    if (!Session) {
        elmprintf(LOGERR, "%s %s: %s",
                  "Unable to", message, "Session object does not exist.");
        return 0;
    }

    return 1;
}
