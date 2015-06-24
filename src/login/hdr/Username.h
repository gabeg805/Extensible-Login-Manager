// Header guard
#ifndef USERNAME_H
#define USERNAME_H

// Includes
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <gtk/gtk.h>

// Defines
#define USER_CONFIG        "/etc/X11/elysia/src/login/config/Username.config"
#define USER_IMG_CONFIG    "/etc/X11/elysia/src/login/config/Username-Image.config"

// Public functions
void display_username();

#endif
