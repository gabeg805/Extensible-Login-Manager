/* *****************************************************************************
 * 
 * Name:    elysia.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "elysia.h"
 * 
 * Description: Global variables used throughout the the Elysia source files.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_H
#define ELYSIA_H

/* Includes */
#include <stdbool.h>
#include "elyapp.h"

/* Global variables */
extern const char *PROG;
extern char *SERVICE;
extern char *USERNAME;
extern char *PASSWORD;
extern char *SESSION;
extern char *ELYSIA_LOG;
extern int  TTYN;
extern bool INTERFACE;
extern bool PREVIEW;
extern bool VERBOSE;

#endif /* ELYSIA_H */
