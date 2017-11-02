/* *****************************************************************************
 * 
 * Name:    elmloginmanager.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Control setting up, building, and displaying the ELM.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_LOGIN_MANAGER_H
#define ELM_LOGIN_MANAGER_H

/* Login manager object */
typedef struct
{
    int    (*run)(void);
    int    (*login_prompt)(void);
    void * (*login_session)(void *arg);
    int    (*build_window)(void);
    int    (*build_apps)(void);
    int    (*setup_dir)(void);
    int    (*setup_xserver)(void);
    int    (*setup_signal_catcher)(void);
    int    (*show_apps)(void);
    int    (*hide_apps)(void);
    void   (*set_preview_mode)(int flag);
} ElmLoginManager;

/* Public functions  */
ElmLoginManager * elm_login_manager_new(void);

#endif /* ELM_LOGIN_MANAGER_H */
