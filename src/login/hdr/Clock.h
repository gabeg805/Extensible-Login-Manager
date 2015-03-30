// Header guard
#ifndef CLOCK_H
#define CLOCK_H

// Includes
#include "../hdr/glm.h"
#include "../hdr/Utility.h"

#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <gtk/gtk.h>

// Defines
#define CLOCK_DATE_CONFIG    "/etc/X11/glm/src/login/config/Clock-Date.config"
#define CLOCK_TIME_CONFIG    "/etc/X11/glm/src/login/config/Clock-Time.config"

#define UPDATE_SEC   30

// Definitions
struct glmtxt_w {
    GtkWidget *widg;
    struct glmtxt txt;
};

// Public functions
void display_clock();

#endif
