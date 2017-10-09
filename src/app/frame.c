/* *****************************************************************************
 * 
 * Name:    frame.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display the login frame for the Extensible Login Manager.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "app/frame.h"
#include "elm.h"

/* Private functions */
static gboolean draw_frame(gpointer data);

/* ************************************************************************** */
/* Create login frame application */
ElmApp * display_frame(ElmCallback callback)
{
    elmprintf(LOG, "Displaying login frame...");

    static GtkWidget *drawing;
    static ElmApp    *frame;
    frame   = elm_new_app();
    drawing = gtk_drawing_area_new();

    frame->new_window(GTK_WINDOW_POPUP);
    frame->set_position(550, 267);
    frame->set_size(270, 175);
    frame->set_self(frame);
    frame->add_container(drawing);
    g_signal_connect(drawing, "draw", G_CALLBACK(draw_frame), NULL);

    return frame;
}

/* ************************************************************************** */
/* Draw login frame */
gboolean draw_frame(gpointer data)
{ 
    GtkWidget *drawing = (GtkWidget*)data;
    cairo_t   *cr      = gdk_cairo_create(gtk_widget_get_window(drawing));
    double     width   = 270;
    double     height  = 175;
    double     curve   = 5;
    double     deg     = M_PI / 180.0;

    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
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
    cairo_destroy(cr);

    return FALSE;
}
