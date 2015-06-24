// Header guard
#ifndef INTERFACE_H
#define INTERFACE_H

#include "elysia.h"
#include "Username.h"
#include "Password.h"
#include "Clock.h"
#include "Frame.h"
#include "TextImage.h"
#include "WindowManager.h"
#include "Panel.h"
#include "utility.h"

#include <stdbool.h>
#include <gtk/gtk.h>

// Public functions
void login_interface(int argc, char *argv[]);

#endif
