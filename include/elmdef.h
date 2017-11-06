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

/* Includes */
#include <errno.h>

/* Program name */
#define PROGRAM "elm"

/* Exit codes */
#define ELM_EXIT_SUCCESS        0
#define ELM_EXIT_INV_OPT        1
#define ELM_EXIT_MNGR_NEW       10
#define ELM_EXIT_MNGR_APP       11
#define ELM_EXIT_MNGR_RUN       12
#define ELM_EXIT_MNGR_DIR       13
#define ELM_EXIT_MNGR_SIG_SETUP 14
#define ELM_EXIT_MNGR_SIG       15
#define ELM_EXIT_MNGR_X         16
#define ELM_EXIT_MNGR_PROMPT    17
#define ELM_EXIT_MNGR_BUILD_WIN 18
#define ELM_EXIT_MNGR_BUILD_APP 19
#define ELM_EXIT_MNGR_PTHREAD   20
#define ELM_EXIT_MNGR_PREVIEW   21
#define ELM_EXIT_X_INIT         22
#define ELM_EXIT_X_ENV_DISPLAY  23
#define ELM_EXIT_X_ENV_TTY      24
#define ELM_EXIT_X_ENV_TTYN     25
#define ELM_EXIT_X_ENV_XAUTH    26
#define ELM_EXIT_X_ENV_VT       27
#define ELM_EXIT_X_EXEC         28
#define ELM_EXIT_X_WAIT         29
#define ELM_EXIT_X_STOP         30
#define ELM_EXIT_X_XCOMPMGR     31
#define ELM_EXIT_SESS_NEW       32
#define ELM_EXIT_SESS_INFO_NEW  33
#define ELM_EXIT_SESS_INFO_HELPER_NEW  34
#define ELM_EXIT_PAM_LOGIN      35

/* Commands */
#define ELM_CMD_SHUTDOWN "/usr/bin/poweroff"
#define ELM_CMD_REBOOT   "/usr/bin/reboot"
#define ELM_CMD_XORG     "/usr/bin/Xorg"
#define ELM_CMD_XCOMPMGR "/usr/bin/xcompmgr"
#define ELM_CMD_XRDB     "/usr/bin/xrdb"
#define ELM_CMD_XMODMAP  "/usr/bin/xmodmap"

/* Paths */
#define ELM_RUN_DIR "/var/run/" PROGRAM
#define ELM_LOG_DIR "/var/log/" PROGRAM
#define ELM_LOG     ELM_LOG_DIR "/elm.log"
#define ELM_XLOG    ELM_LOG_DIR "/Xorg.log"

/* Sizes */
#define ELM_MAX_CRED_SIZE 64
#define ELM_MAX_PATH_SIZE 128
#define ELM_MAX_CMD_SIZE  128
#define ELM_MAX_MSG_SIZE  256

#endif /* ELM_DEF_H */
