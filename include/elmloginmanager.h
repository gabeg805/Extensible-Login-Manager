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

/* Typedefs */
typedef struct
{
    int    (*run)(void);
    void * (*login_session)(void *arg);
    int    (*login_prompt)(void);
    int    (*xinit)(void);
    int    (*xstyle)(void);
    int    (*setup_signal_catcher)(void);
    int    (*build_window)(void);
    int    (*build_apps)(void);
    int    (*show_apps)(void);
    int    (*hide_apps)(void);
    void   (*set_preview_mode)(int flag);
} ElmLoginManager;

/* Public functions  */
ElmLoginManager * elm_new_login_manager(void);

#endif /* ELM_LOGIN_MANAGER_H */
