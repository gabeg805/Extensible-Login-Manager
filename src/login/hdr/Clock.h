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
#define CLOCK_DATE_PREF    "/etc/X11/glm/src/login/pref/Clock-Date.pref"
#define CLOCK_TIME_PREF    "/etc/X11/glm/src/login/pref/Clock-Time.pref"

#define UPDATE_SEC   30

// Definitions
struct glmtxt_w {
    GtkWidget *widg;
    struct glmtxt txt;
};

// Public functions
void display_clock();

#endif
