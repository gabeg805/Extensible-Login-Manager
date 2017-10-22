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
#include <stdlib.h>

/* Defines */
#define ELM_EXIT_SESSION_NEW    100
#define ELM_EXIT_SESSION_AUTH   110
#define ELM_EXIT_SESSION_LOGIN  120
#define ELM_EXIT_SESSION_LOGOUT 130

/* Typedefs */
typedef struct
{
    char username[64];
    char password[64];
    char xsession[64];
} ElmLogin;

typedef struct
{
    int      (*authenticate)(void);
    int      (*login)(void);
    int      (*logout)(void);
    ElmLogin  *info;
    pid_t      pid;
} ElmSession;

/* Public functions  */
ElmSession * elm_session_new(ElmLogin *info);

#endif /* ELM_SESSION_H */
