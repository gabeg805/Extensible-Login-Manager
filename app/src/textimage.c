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
#include "elysia.h"
#include "elyapp.h"
#include "elytype.h"
#include "utility.h"
#include "benchmark.h"
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
    double bmtime = benchmark_runtime(0);

    cairo_select_font_face(cr, APP.txt.font, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, APP.txt.size);
    cairo_set_source_rgb(cr, APP.txt.red, APP.txt.green, APP.txt.blue);
    cairo_move_to(cr, 0, APP.txt.size);
    cairo_show_text(cr, APP.txt.text);

    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}



/* Draw the root window */
static gboolean draw_window(GtkWidget *widg) {
    double bmtime = benchmark_runtime(0);

    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widg));
    cairo_set_source_rgba(cr, 0, 0, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    draw_text(cr);
    cairo_destroy(cr);

    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));

    return FALSE;
}



/* ****************************** */
/* ***** DISPLAY TEXT IMAGE ***** */
/* ****************************** */

/* Dislay the text image */
void display_text_image() {
    double bmtime = benchmark_runtime(0);
    if ( VERBOSE )
        file_log("%s: (%s:%d): Displaying text image...", 
                 __FILE__, __FUNCTION__, __LINE__);

    /* Create the text image application */
    APP.win  = gtk_window_new(GTK_WINDOW_POPUP);
    APP.widg = gtk_drawing_area_new(); 
    setup_app(TEXT_CONFIG, &APP, "draw", (void *)draw_window);

    if ( VERBOSE )
        file_log("Done\n");
    if ( BENCHTIME )
        file_log("%s: (%s: Runtime): %lf\n", 
                 __FILE__, __FUNCTION__, benchmark_runtime(bmtime));
}
