/* *****************************************************************************
 * 
 * Name:    frame.c
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
#include "elyapp.h"
#include "elyglobal.h"
#include "elytype.h"
#include "utility.h"
#include <math.h>
#include <cairo.h>
#include <gtk/gtk.h>

/* Private functions */
static void draw_frame(cairo_t *cr);
static gboolean draw_window(GtkWidget *drawing);

/* Declares */
static struct elyapp frame;

/* *************************
 * ***** DISPLAY FRAME *****
 * *************************
 */
/* Display the login frame */
void display_frame()
{
    TRACE(stdout, "%s", "Displaying login frame application...");

    /* Create the login frame */
    frame.gui.win  = gtk_window_new(GTK_WINDOW_POPUP);
    frame.gui.widg = gtk_drawing_area_new();
    setup_app_settings(&frame, FRAME_CONFIG, NULL, NULL);
    set_app(&frame);
    g_signal_connect(frame.gui.widg, "draw", (void*)draw_window, NULL);

    TRACE(stdout, "%s", "Done displaying frame.");
}

/* **********************
 * ***** DRAW FRAME *****
 * **********************
 */
/* Draw the login frame */
static void draw_frame(cairo_t *cr)
{ 
    TRACE(stdout, "%s", "Drawing login frame...");

    double width  = frame.shape.width;
    double height = frame.shape.height;
    double curve  = frame.shape.curve;
    double deg    = M_PI / 180.0;
    cairo_set_line_width (cr, 0);
    cairo_new_sub_path(cr);
    cairo_arc(cr, width-curve, curve,        curve, -90*deg,   0*deg);
    cairo_arc(cr, width-curve, height-curve, curve,   0*deg,  90*deg);
    cairo_arc(cr, curve,       height-curve, curve,  90*deg, 180*deg);
    cairo_arc(cr, curve,       curve,        curve, 180*deg, 270*deg);
    cairo_close_path (cr);
    cairo_set_source_rgba(cr, 1, 1, 1, 0.5);
    cairo_fill_preserve(cr);
    cairo_stroke (cr);

    TRACE(stdout, "%s", "Done drawing frame.");
}

/* Draw the root window  */
static gboolean draw_window(GtkWidget *drawing)
{
    TRACE(stdout, "%s", "Drawing login frame window...");

    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(drawing));
    cairo_set_source_rgba(cr, 0, 0, 0, 0);    
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    draw_frame(cr);
    cairo_destroy(cr);

    TRACE(stdout, "%s", "Done drawing frame window.");

    return FALSE;
}
