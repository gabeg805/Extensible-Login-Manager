/* *****************************************************************************
 * 
 * Name:    elmsession.h
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

/* Header guard */
#ifndef ELM_SESSION_H
#define ELM_SESSION_H

/* Includes */
#include "elmdef.h"
#include <stdlib.h>
#include <gtk/gtk.h>

/* Login information helper */
typedef struct
{
    GtkWidget *widget;
    char      *data;
} ElmSessionInfoHelper;

/* Login information */
typedef struct
{
    char  username[ELM_MAX_CRED_SIZE];
    char  password[ELM_MAX_CRED_SIZE];
    char  xsession[ELM_MAX_CRED_SIZE];
} ElmSessionInfo;

/* Session object */
typedef struct
{
    int            (*auth)(void);
    int            (*login)(void);
    int            (*logout)(void);
    ElmSessionInfo  *info;
} ElmSession;

/* Public functions  */
ElmSession           * elm_session_new(ElmSessionInfo *info);
ElmSessionInfo       * elm_session_info_new(void);
ElmSessionInfoHelper * elm_session_info_helper_new(GtkWidget *widget,
                                                   char *data);

#endif /* ELM_SESSION_H */
