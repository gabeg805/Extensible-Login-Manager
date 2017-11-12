/* *****************************************************************************
 * 
 * Name:    elmstd.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Standard system functions.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmstd.h"
#include "elmio.h"
#include <stdlib.h>
#include <unistd.h>

/* -------------------------------------------------------------------------- */
/* Execute a command/file */
int elm_std_execvp(char *file, char *const argv[])
{
    elmprintf(LOGINFO, "%s '%s'.", "Running", file);
    execvp(file, argv);
    elmprintf(LOGERRNO, "%s '%s'", "Error trying to run", file);

    return -1;
}

/* -------------------------------------------------------------------------- */
/* Set environment variables */
int elm_std_setenv(char *name, char *value)
{
    elmprintf(LOGINFO, "Setting environment variable: '%s=%s'.", name, value);

    if (setenv(name, value, 1) < 0) {
        elmprintf(LOGERRNO, "%s '%s=%s'",
                  "Unable to set environment variable", name, value);
        return -1;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */
/* Completely free memory */
void elm_std_free(void *ptr)
{
    char **ref = ptr;

    free(*ref);

    *ref = NULL;
}
