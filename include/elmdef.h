/* *****************************************************************************
 * 
 * Name:    elmdef.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Define macros for ELM.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_DEF_H
#define ELM_DEF_H

/* Defines */
#define PROGRAM "elm"

/* Defines */
#define ELM_EXIT_SUCCESS                               0
#define ELM_EXIT_INVALID_OPTION                        1

#define ELM_EXIT_LOGIN_MANAGER_NEW                     1
#define ELM_EXIT_LOGIN_MANAGER_APP                     2
#define ELM_EXIT_LOGIN_MANAGER_RUN                     3
#define ELM_EXIT_LOGIN_MANAGER_SETUP_SIGNAL_CATCHER    4
#define ELM_EXIT_LOGIN_MANAGER_SIGNAL_CATCHER          5
#define ELM_EXIT_LOGIN_MANAGER_XINIT                   6
#define ELM_EXIT_LOGIN_MANAGER_XSTYLE                  7
#define ELM_EXIT_LOGIN_MANAGER_PROMPT                  8

#define ELM_EXIT_X_START    10
#define ELM_EXIT_X_OPEN     11
#define ELM_EXIT_X_WAIT     12
#define ELM_EXIT_X_STOP     13
#define ELM_EXIT_X_DISPLAY  14
#define ELM_EXIT_X_TTY      15
#define ELM_EXIT_X_XCOMPMGR 16


#define ELM_VAR_RUN_DIR                                "/var/run/" PROGRAM

#define ELM_CMD_SHUTDOWN "/usr/bin/poweroff"
#define ELM_CMD_REBOOT   "/usr/bin/reboot"
#define ELM_CMD_XORG     "/usr/bin/Xorg"
#define ELM_CMD_XCOMPMGR "/usr/bin/xcompmgr"
#define ELM_CMD_XRDB     "/usr/bin/xrdb"
#define ELM_CMD_XMODMAP  "/usr/bin/xmodmap"


#define ELM_LOG  "/var/log/elm/elm.log"
#define ELM_XLOG "/var/log/elm/Xorg.log"

#endif /* ELM_DEF_H */
