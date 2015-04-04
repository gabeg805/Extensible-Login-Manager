// Header guard
#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

// Includes
#include "../hdr/glm.h"
#include "../hdr/Utility.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <security/pam_appl.h>
#include <security/pam_misc.h>

// Defines
#define AUTH_CONFIG    "/etc/X11/glm/src/login/config/Authenticate.config"

#define SESSREG    "/usr/bin/sessreg"
#define UTMP_ADD   "/run/utmp"
#define UTMP_DEL   "/var/run/utmp"
#define WTMP       "/var/log/wtmp"
#define XINITRC    "/etc/X11/glm/src/x/xinitrc"

// Public functions
int login(const char *username, const char *password);

#endif
