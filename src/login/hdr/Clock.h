// Header guard
#ifndef CLOCK_H
#define CLOCK_H

// Includes
#include "utility.h"

#include <assert.h>
#include <time.h>
#include <gtk/gtk.h>

// Defines
#define CLOCK_DATE_CONFIG    "/etc/X11/elysia/src/login/config/Clock-Date.config"
#define CLOCK_TIME_CONFIG    "/etc/X11/elysia/src/login/config/Clock-Time.config"

// Public functions
void display_clock();

#endif
