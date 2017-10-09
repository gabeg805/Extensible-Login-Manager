/* *****************************************************************************
 * 
 * Name:    elmsession.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Create a user session for the Extensible Login Manager.
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
    char *username;
    char *password;
    char *xsession;
} ElmLoginInfo;

typedef struct
{
    int (*authenticate)(void);
    int (*login)(void);
    int (*logout)(void);

    ElmLoginInfo *_info;
    pid_t         pid;
} ElmSession;

/* Public functions  */
ElmSession * elm_new_session(ElmLoginInfo*);

#endif /* ELM_SESSION_H */
