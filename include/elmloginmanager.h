/* *****************************************************************************
 * 
 * Name:    elmloginmanager.h
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

/* Header guard */
#ifndef ELM_LOGIN_MANAGER_H
#define ELM_LOGIN_MANAGER_H

/* Includes */
#include <stdlib.h>
#include <gtk/gtk.h>

/* Defines */
#define ELM_EXIT_LOGIN_MANAGER_NEW    1
#define ELM_EXIT_LOGIN_MANAGER_APP    2
#define ELM_EXIT_LOGIN_MANAGER_RUN    3
#define ELM_EXIT_LOGIN_MANAGER_XINIT  4
#define ELM_EXIT_LOGIN_MANAGER_PROMPT 5

/* Typedefs */
typedef struct
{
    int    (*run)(void);
    int    (*xinit)(void);
    int    (*prompt)(void);
    void * (*usersession)(void*);
} ElmLoginManager;

/* Public functions  */
ElmLoginManager * elm_new_login_manager(int argc, char **argv);

#endif /* ELM_LOGIN_MANAGER_H */
