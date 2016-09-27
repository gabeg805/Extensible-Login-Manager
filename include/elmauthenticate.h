/* *****************************************************************************
 * 
 * Name:    elmauthenticate.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Authenticate username and password for the Extensible Login
 *              Manager.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_AUTHENTICATE_H
#define ELM_AUTHENTICATE_H

/* Includes */
#include <unistd.h>

/* Defines */
#define SERVICE "elm"
#define ELM_EXIT_AUTHENTICATE_PAM     20
#define ELM_EXIT_AUTHENTICATE_LOGIN   21
#define ELM_EXIT_AUTHENTICATE_PREVIEW 22

/* Public functions */
int elm_authenticate(const char *username, const char *password);
int elm_login(const char *session, pid_t *parentpid);
int elm_logout(void);

#endif /* ELM_AUTHENTICATE_H */
