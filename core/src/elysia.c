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
#include "elyglobal.h"
#include "xserver.h"
#include "interface.h"
#include "authenticate.h"
#include "utility.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

/* Globals */
const char *PROG;
char *SERVICE     = "elysia";
char *USERNAME    = "User";
char *PASSWORD    = "Password";
char *SESSION     = "xterm";
char *ELYSIA_LOG  = "/etc/X11/elysia/elysia.log";
int  TTYN         = 1;
bool PREVIEW      = false;
bool VERBOSE      = false;
bool BENCHTIME    = false;



/* ******************************** */
/* ***** ELYSIA LOGIN MANAGER ***** */
/* ******************************** */

/* Display the Elysia Login Manager  */
int main(int argc, char **argv)
{
    PROG = argv[0];
    parse_argv(argc, argv);

    /* Log program start */
    time_t t;
    time(&t);
    TRACE(stdout, "**************************************************", "");
    TRACE(stdout, "Date: %s", ctime(&t));

    xsetup();

    while (true) {
        login_interface();
        if ( login(USERNAME, PASSWORD) )
            break;
    }

    return 0;
}
