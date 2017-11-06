/* *****************************************************************************
 * 
 * Name:    elmpam.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Authenticate username and password using the Pluggable
 *              Authentication Module (PAM).
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_PAM_H
#define ELM_PAM_H

/* Includes */
#include "elmsession.h"

/* Public functions */
int elm_pam_init(ElmSessionInfo *info);
int elm_pam_auth(void);
int elm_pam_login(void);
int elm_pam_logout(void);

#endif /* ELM_PAM_H */
