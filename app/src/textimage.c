/* *****************************************************************************
 * 
 * Name:    textimage.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Display the text image application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "textimage.h"
#include "elyglobal.h"
#include "elyapp.h"
#include "elytype.h"
#include "utility.h"
#include <cairo.h>
#include <gtk/gtk.h>

/* Private functions */
static void draw_text(cairo_t *cr);
static gboolean draw_window(GtkWidget *window);

/* Application */
static struct elyapp APP;



/* *************************** */
/* ***** DRAW TEXT IMAGE ***** */
/* *************************** */

/* Draw the text */
static void draw_text(cairo_t *cr) {         
    TRACE(stdout, "%s", "Drawing text...");

    cairo_select_font_face(cr, APP.txt.font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, APP.txt.size);
    cairo_set_source_rgb(cr, APP.txt.red, APP.txt.green, APP.txt.blue);
    cairo_move_to(cr, 0, APP.txt.size);
    cairo_show_text(cr, APP.txt.text);

    TRACE(stdout, "%s", "Done drawing text.");
}



/* Draw the root window */
static gboolean draw_window(GtkWidget *widg) {
    TRACE(stdout, "%s", "Drawing text window...");

    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widg));
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    draw_text(cr);
    cairo_destroy(cr);

    TRACE(stdout, "%s", "Done drawing text window.");

    return FALSE;
}



/* ****************************** */
/* ***** DISPLAY TEXT IMAGE ***** */
/* ****************************** */

/* Dislay the text image */
void display_text_image() {
    TRACE(stdout, "%s", "Displaying text application...");

    /* Create the text image application */
    APP.win  = gtk_window_new(GTK_WINDOW_POPUP);
    APP.widg = gtk_drawing_area_new(); 
    setup_app(TEXT_CONFIG, &APP, "draw", (void *)draw_window);

    TRACE(stdout, "%s", "Done displaying text.");
}
