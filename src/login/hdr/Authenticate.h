#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>

#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

int login(const char *username, const char *password);

#endif
