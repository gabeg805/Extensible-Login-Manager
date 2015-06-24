/* *****************************************************************************
 * 
 * Name:    elysia.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: # ./elysia [option]
 * 
 * Description: Display the Elysia Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "Xsetup.h"
#include "Username.h"
#include "Password.h"
#include "Interface.h"
#include "Authenticate.h"
#include "utility.h"
#include "benchmark.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* Globals */
char *SERVICE  = "elysia";
char *USERNAME = "User";
char *PASSWORD = "Password";
char *SESSION  = "xterm";
char *ELYSIA_LOG  = "/etc/X11/elysia/log/elysia.log";
int  TTYN      = 1;
bool INTERFACE = false;
bool PREVIEW   = false;
bool VERBOSE   = false;
bool BENCHTIME = false;



// ////////////////////////////////
// ///// GABE'S LOGIN MANAGER /////
// ////////////////////////////////

// Display the Elysia Login Manager 
int main(int argc, char **argv)
{
    parse_argv(argc, argv);

    // Log program start
    if ( VERBOSE || BENCHTIME ) {
        time_t t;
        time(&t);
        file_log("\nDate: %s", ctime(&t));
    }

    xsetup();

    while (true) {
        login_interface(argc, argv);
        if ( login(USERNAME, PASSWORD) )
            break;
    }

    return 0;
}
