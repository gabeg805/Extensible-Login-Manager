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

/* Typedefs */
typedef struct
{
    char  username[ELM_MAX_CRED_SIZE];
    char  password[ELM_MAX_CRED_SIZE];
    char  xsession[ELM_MAX_CRED_SIZE];
} ElmLogin;

typedef struct
{
    int      (*auth)(void);
    int      (*login)(void);
    int      (*logout)(void);
    ElmLogin  *info;
} ElmSession;

/* Public functions  */
ElmSession * elm_session_new(ElmLogin *info);

#endif /* ELM_SESSION_H */
