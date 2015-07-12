/* *****************************************************************************
 * 
 * Name:    elytype.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "elytype.h"
 * 
 * Description: Struct definitions used by all Elysia source files.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_ELYTYPE_H
#define ELYSIA_ELYTYPE_H

/* Includes */
#include <gtk/gtk.h>

/* Definitions */
/* Settings file paths */
struct elysettings {
    char *config;
    char *style;
    char *class;
};

/* Application position */
struct elypos {
    int x;
    int y;
};

/* Application shape attributes */
struct elyshape {
    int width;
    int height;
    int curve;
    char *img;
};

/* Text attributes */
struct elytext {
    char *fmt;
    char *invis;
    int maxchars;
};

/* GUI attributes */
struct elygui {
    GtkWidget *win;
    GtkWidget *widg;
};

/* Application */
struct elyapp {
    struct elygui gui;
    struct elypos pos;
    struct elyshape shape;
    struct elytext text;
    struct elysettings settings;
    int refresh;
};

#endif /* ELYSIA_ELYTYPE_H */
