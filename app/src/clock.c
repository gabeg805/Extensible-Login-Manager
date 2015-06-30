/* *****************************************************************************
 * 
 * Name:    clock.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: #include "clock.h"
 * 
 * Description: Display the clock application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "clock.h"
#include "elyglobal.h"
#include "elyapp.h"
#include "elytype.h"
#include "utility.h"
#include <time.h>
#include <gtk/gtk.h>

/* Private functions */
static void set_label(struct elyapp *app);
static gboolean update(gpointer data);
static void display_item(char *file, struct elyapp *app);



/* ************************** */
/* ***** SET CLOCK TIME ***** */
/* ************************** */

/* Set the clock label font and text size */
static void set_label(struct elyapp *app) {
    TRACE(stdout, "%s", "Setting clock label...");

    time_t current = time(0);
    struct tm *t   = localtime(&current);
    char time[50];
    strftime(time, sizeof(time), app->txt.fmt, t);

    PangoAttrList *attrList  = pango_attr_list_new();
    PangoAttribute *attrFont = pango_attr_family_new(app->txt.font);
    PangoAttribute *attrSize = pango_attr_size_new((long)1024 * app->txt.size);
    attrList                 = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrFont);
    gtk_label_set_attributes(GTK_LABEL(app->widg), attrList);
    gtk_label_set_text(GTK_LABEL(app->widg), time);

    TRACE(stdout, "%s", "Done setting clock label.");
}



/* ************************ */
/* ***** UPDATE CLOCK ***** */
/* ************************ */

/* Refresh the current clock label */
static gboolean update(gpointer data) {
    TRACE(stdout, "%s", "Updating clock...");

    struct elyapp *app = (struct elyapp *) data;
    set_label(app);

    TRACE(stdout, "%s", "Done updating clock.");

    return TRUE;
}



/* ************************* */
/* ***** DISPLAY CLOCK ***** */
/* ************************* */

/* Display a piece of the time */
static void display_item(char *file, struct elyapp *app) {
    TRACE(stdout, "%s", "Displaying clock item...");

    /* Create the clock application */
    app->win  = gtk_window_new(GTK_WINDOW_POPUP);
    app->widg = gtk_label_new("");
    setup_app(file, app, 0, 0);
    set_label(app);
    g_timeout_add_seconds(app->txt.refresh, update, app);

    TRACE(stdout, "%s", "Done displaying clock item.");
}



/* Display the date and time clock */
void display_clock() {
    static struct elyapp date_app;
    static struct elyapp time_app;
    display_item(CLOCK_DATE_CONFIG, &date_app);
    display_item(CLOCK_TIME_CONFIG, &time_app);
}
