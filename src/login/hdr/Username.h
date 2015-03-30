// Header guard
#ifndef USERNAME_H
#define USERNAME_H

// Includes
#include "../hdr/glm.h"
#include "../hdr/Utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <gtk/gtk.h>

// Defines
#define USER_CONFIG        "/etc/X11/glm/src/login/config/Username-User.config"
#define USER_IMG_CONFIG    "/etc/X11/glm/src/login/config/Username-Image.config"

#define USERNAME_LOG   "/etc/X11/glm/log/user.log"

// Public functions
void display_username();

#endif
