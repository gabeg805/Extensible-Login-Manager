/* *****************************************************************************
 * 
 * Name:    elytypes.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "elytypes.h"
 * 
 * Description: Struct definitions used by all Elysia source files.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELYSIA_ELYTYPES_H
#define ELYSIA_ELYTYPES_H

/* Includes */
#include <gtk/gtk.h>

/* Definitions */
/* Widget position and size */
struct elypos {
    int x;
    int y;
    int width;
    int height;
};

/* Text attributes */
struct elytxt {
    int size;
    int maxchars;
    int refresh;
    
    char *text;
    char *font;
    char *fmt;
    char *invis;
    
    int red;
    int green;
    int blue;
};

/* Widget decoration */
struct elydecor {
    char *img_file;
    
    int bg_red;
    int bg_green;
    int bg_blue;
    int bg_alpha;
    
    int fg_red;
    int fg_green;
    int fg_blue;
    int fg_alpha;
    
    int div;
};

/* Widget application */
struct elyapp {
    GtkWidget *win;
    GtkWidget *widg;
    struct elypos pos;
    struct elytxt txt;
    struct elydecor decor;
};

#endif /* ELYSIA_ELYTYPES_H */
