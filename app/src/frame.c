/* *****************************************************************************
 * 
 * Name:    utility.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Display the login frame application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "frame.h"
#include "elyglobal.h"
#include "elyapp.h"
#include "elytype.h"
#include "utility.h"
#include <math.h>
#include <cairo.h>
#include <gtk/gtk.h>


/* Private functions */
static void draw_frame(cairo_t *cr);
static gboolean draw_window(GtkWidget *widget);

/* Declares */
static struct elyapp APP;



/* ********************** */
/* ***** DRAW FRAME ***** */
/* ********************** */

/* Draw the login frame */
static void draw_frame(cairo_t *cr) { 
    TRACE(stdout, "%s", "Drawing login frame...");

    double width  = APP.pos.width;
    double height = APP.pos.height;
    double rad    = height / 5.0;
    double deg    = M_PI / 180.0;
    cairo_set_line_width (cr, 0);
    cairo_new_sub_path(cr);
    cairo_arc(cr, width-rad, rad,        rad, -90*deg,   0*deg);
    cairo_arc(cr, width-rad, height-rad, rad,   0*deg,  90*deg);
    cairo_arc(cr, rad,       height-rad, rad,  90*deg, 180*deg);
    cairo_arc(cr, rad,       rad,        rad, 180*deg, 270*deg);
    cairo_close_path (cr);
    cairo_set_source_rgba(cr, 1, 1, 1, 0.5);
    cairo_fill_preserve(cr);
    cairo_stroke (cr);

    TRACE(stdout, "%s", "Done drawing frame.");
}




/* Draw the root window  */
static gboolean draw_window(GtkWidget *widg) {
    TRACE(stdout, "%s", "Drawing login frame window...");

    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widg));
    cairo_set_source_rgba(cr, 0, 0, 0, 0);    
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    draw_frame(cr);
    cairo_destroy(cr);

    TRACE(stdout, "%s", "Done drawing frame window.");

    return FALSE;
}



/* ************************* */
/* ***** DISPLAY FRAME ***** */
/* ************************* */

/* Display the login frame */
void display_frame() {
    TRACE(stdout, "%s", "Displaying login frame application...");

    /* Create the login frame */
    APP.win  = gtk_window_new(GTK_WINDOW_POPUP);
    APP.widg = gtk_drawing_area_new(); 
    setup_app(FRAME_CONFIG, &APP, "draw", (void *)draw_window);

    TRACE(stdout, "%s", "Done displaying frame.");
}
