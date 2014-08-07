#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

int is_pam_success(int result, pam_handle_t *pamh);
int login(const char *username, const char *password);
int conv(int num_msg, const struct pam_message **msg, struct pam_response **resp, void *appdata_ptr);

#endif
