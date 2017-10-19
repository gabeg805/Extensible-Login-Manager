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
#include <gdk/gdk.h>

/* Typedefs */
typedef struct
{
    int    (*run)(void);
    void * (*login_session)(void *arg);
    int    (*login_prompt)(void);
    int    (*build_window)(void);
    int    (*build_apps)(void);
    int    (*show_apps)(void);
    int    (*hide_apps)(void);
    int    (*xsetup)(void);
    int    (*setup_signal_catcher)(void);
    void   (*set_preview_mode)(int flag);
} ElmLoginManager;

/* Public functions  */
ElmLoginManager * elm_new_login_manager(void);

#endif /* ELM_LOGIN_MANAGER_H */
