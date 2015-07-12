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
#include "elyapp.h"
#include "elyglobal.h"
#include "elytype.h"
#include "utility.h"
#include <time.h>
#include <gtk/gtk.h>

/* Private functions */
static void display_item(struct elyapp *app);
static void set_label(struct elyapp *app);
static gboolean update(gpointer data);

/* *************************
 * ***** DISPLAY CLOCK *****
 * *************************
 */
/* Display the date and time clock */
void display_clock()
{
    static struct elyapp date_app;
    static struct elyapp time_app;
    setup_app_settings(&date_app, DATE_CONFIG, DATE_STYLE, "Date");
    setup_app_settings(&time_app, TIME_CONFIG, TIME_STYLE, "Time");
    display_item(&date_app);
    display_item(&time_app);
}

/* Display a piece of the time */
static void display_item(struct elyapp *app)
{
    TRACE(stdout, "%s", "Displaying clock item...");

    /* Create the clock application */
    app->gui.win  = gtk_window_new(GTK_WINDOW_POPUP);
    app->gui.widg = gtk_label_new("");
    set_app(app);
    set_label(app);
    set_app_style(app, app->gui.win);
    g_timeout_add_seconds(30, update, app); /* ~Fix: Read update time from config */

    TRACE(stdout, "%s", "Done displaying clock item.");
}

/* **************************
 * ***** SET CLOCK TIME *****
 * **************************
 */
/* Set the clock label font and text size */
static void set_label(struct elyapp *app)
{
    TRACE(stdout, "%s", "Setting clock label...");

    time_t current = time(0);
    struct tm *tp   = localtime(&current);
    char time[50];
    strftime(time, sizeof(time), app->text.fmt, tp);

    gtk_label_set_text(GTK_LABEL(app->gui.widg), time);

    TRACE(stdout, "%s", "Done setting clock label.");
}

/* ************************
 * ***** UPDATE CLOCK *****
 * ************************
 */
/* Refresh the current clock label */
static gboolean update(gpointer data)
{
    TRACE(stdout, "%s", "Updating clock...");
    set_label((struct elyapp *)data);
    TRACE(stdout, "%s", "Done updating clock.");

    return TRUE;
}
