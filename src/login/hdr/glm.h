// Header guard
#ifndef GLM_H
#define GLM_H

// Includes
#include "../hdr/Xsetup.h"
#include "../hdr/Username.h"
#include "../hdr/Password.h"
#include "../hdr/Interface.h"
#include "../hdr/Authenticate.h"
#include "../hdr/Utility.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Global variables
extern char *SERVICE;
extern char *USERNAME;
extern char *PASSWORD;
extern char *SESSION;
extern char *GLM_LOG;
extern int TTY_N;
extern bool INTERFACE;
extern bool PREVIEW;

#endif
