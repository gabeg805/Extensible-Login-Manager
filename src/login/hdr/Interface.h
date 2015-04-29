// Header guard
#ifndef INTERFACE_H
#define INTERFACE_H

#include "../hdr/elysia.h"
#include "../hdr/Username.h"
#include "../hdr/Password.h"
#include "../hdr/Clock.h"
#include "../hdr/Frame.h"
#include "../hdr/TextImage.h"
#include "../hdr/WindowManager.h"
#include "../hdr/Panel.h"
#include "../hdr/Utility.h"

#include <stdbool.h>
#include <gtk/gtk.h>

// Public functions
void login_interface(int argc, char *argv[]);

#endif
