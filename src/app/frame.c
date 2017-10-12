/* *****************************************************************************
 * 
 * Name:    frame.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display the login frame.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "app/frame.h"
#include <math.h>

/* Private functions */
static gboolean draw_frame(gpointer data);

/* ************************************************************************** */
/* Create login frame application */
GtkWidget * display_frame(ElmCallback callback)
{
    elmprintf(LOG, "Displaying login frame.");

    static GtkWidget *drawing;
    drawing = gtk_drawing_area_new();

    elm_set_widget_size(&drawing, 270, 155);
    g_signal_connect(drawing, "draw", G_CALLBACK(draw_frame), NULL);

    return drawing;
}

/* ************************************************************************** */
/* Draw login frame */
gboolean draw_frame(gpointer data)
{ 
    elmprintf(LOG, "Drawing login frame.");

    GtkWidget *drawing = data;
    GdkWindow *window  = elm_get_gdkwindow(&drawing);
    guint      width   = gtk_widget_get_allocated_width(drawing);
    guint      height  = gtk_widget_get_allocated_height(drawing);
    double     curve   = 10;
    double     deg     = M_PI / 180.0;

    if (window == NULL) {
        elmprintf(LOG, "Unable to draw frame: Window returned NULL.");
        return TRUE;
    }

    cairo_region_t    *region  = cairo_region_create();
    GdkDrawingContext *context = gdk_window_begin_draw_frame(window, region);
    cairo_t           *cr      = gdk_drawing_context_get_cairo_context(context);

    cairo_arc(cr, 1.0*width-curve, curve,            curve, -90*deg,   0*deg);
    cairo_arc(cr, 1.0*width-curve, 1.0*height-curve, curve,   0*deg,  90*deg);
    cairo_arc(cr, curve,           1.0*height-curve, curve,  90*deg, 180*deg);
    cairo_arc(cr, curve,           curve,            curve, 180*deg, 270*deg);
    cairo_set_source_rgba(cr, 1, 1, 1, 0.75);
    cairo_fill(cr);
    gdk_window_end_draw_frame(window, context);
    cairo_region_destroy(region);

    return FALSE;
}
