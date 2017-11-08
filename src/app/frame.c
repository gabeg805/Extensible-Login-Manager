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
static gboolean draw_frame(GtkWidget *drawing, cairo_t *cr, gpointer data);

/* Private variables */
static const char *Style = "/etc/X11/elm/share/css/frame.css";

/* ************************************************************************** */
/* Create login frame application */
GtkWidget * new_frame_widget(void)
{
    elmprintf(LOGINFO, "Displaying login frame.");

    GtkWidget *container;
    GtkWidget *drawing;

    container = gtk_fixed_new();
    drawing   = gtk_drawing_area_new();

    elm_gtk_set_widget_size(&drawing, 270, 150);
    gtk_fixed_put(GTK_FIXED(container), drawing, 0, 0);
    g_signal_connect(drawing, "draw", G_CALLBACK(draw_frame), NULL);
    gtk_widget_show(drawing);
    gtk_widget_show(container);

    return container;
}

/* ************************************************************************** */
/* Draw login frame */
gboolean draw_frame(GtkWidget *drawing, cairo_t *cr, gpointer data)
{
    int    width   = gtk_widget_get_allocated_width(drawing);
    int    height  = gtk_widget_get_allocated_height(drawing);
    double curve   = 10;
    double deg     = M_PI / 180.0;

    /* Render background */
    GtkStyleContext *context = gtk_widget_get_style_context(drawing);

    elm_gtk_set_widget_style(&drawing, "Frame", Style);
    gtk_render_background(context, cr, 0, 0, width, height);

    /* Curved edges */
    cairo_arc(cr, 1.0*width-curve, curve,            curve, -90*deg,   0*deg);
    cairo_arc(cr, 1.0*width-curve, 1.0*height-curve, curve,   0*deg,  90*deg);
    cairo_arc(cr, curve,           1.0*height-curve, curve,  90*deg, 180*deg);
    cairo_arc(cr, curve,           curve,            curve, 180*deg, 270*deg);

    /* Set frame color */
    GtkStateFlags flags = gtk_style_context_get_state(context);
    GdkRGBA       color;

    gtk_style_context_get_color(context, flags, &color);
    gdk_cairo_set_source_rgba(cr, &color);

    /* Draw frame */
    cairo_fill(cr);

    return FALSE;
}
