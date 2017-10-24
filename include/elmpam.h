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
#include <unistd.h>

/* Defines */
#define SERVICE "elm"
#define ELM_EXIT_AUTHENTICATE_PAM     20
#define ELM_EXIT_AUTHENTICATE_LOGIN   21
#define ELM_EXIT_AUTHENTICATE_PREVIEW 22

/* Public functions */
int elm_login(char *username, char *xsession, pid_t *parentpid);
int elm_logout(void);
int elm_authenticate(const char *username, const char *password);

#endif /* ELM_PAM_H */
