/* *****************************************************************************
 * 
 * Name:    datetime.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display the date and time.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "app/datetime.h"
#include <time.h>

/* Private functions */
static gboolean set_label_date(gpointer data);
static gboolean set_label_time(gpointer data);

/* Private variables */
static const char *Style = "/etc/X11/elm/share/css/datetime.css";

/* ************************************************************************** */
/* Create date and time application */
GtkWidget * display_datetime(ElmCallback callback)
{
    elmprintf(LOGINFO, "Displaying date and time.");

    /* Create widgets */
    static GtkWidget *box  = NULL;
    static GtkWidget *date = NULL;
    static GtkWidget *time = NULL;

    box  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    date = gtk_label_new("");
    time = gtk_label_new("");

    /* Set refresh rate of time */
    int sec;

    if ((sec=elm_conf_read_int("Datetime", "RefreshTime")) < 0) {
        sec = 5;
    }

    /* Finish setting up widgets */
    gtk_box_pack_start(GTK_BOX(box), time, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), date, TRUE, TRUE, 0);

    gtk_widget_set_halign(date, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(time, GTK_ALIGN_CENTER);
    elm_gtk_set_widget_style(&date, "Date", Style);
    elm_gtk_set_widget_style(&time, "Time", Style);
    set_label_date(&date);
    set_label_time(&time);

    g_timeout_add_seconds(sec, set_label_date, &date);
    g_timeout_add_seconds(sec, set_label_time, &time);

    gtk_widget_show(date);
    gtk_widget_show(time);
    gtk_widget_show(box);

    return box;
}

/* ************************************************************************** */
/* Set date */
gboolean set_label_date(gpointer data)
{
    GtkWidget **label  = (GtkWidget**) data;
    time_t      now    = time(NULL);
    struct tm  *tm     = localtime(&now);
    char       *format;
    char        string[64];

    if (!(format=elm_conf_read("Datetime", "DateFormat"))) {
        format = "%A, %B %-d";
    }

    strftime(string, sizeof(string), format, tm);
    gtk_label_set_text(GTK_LABEL(*label), string);

    return TRUE;
}

/* ************************************************************************** */
/* Set time */
gboolean set_label_time(gpointer data)
{
    GtkWidget **label = (GtkWidget**) data;
    time_t      now   = time(NULL);
    struct tm  *tm    = localtime(&now);
    char       *format;
    char        string[64];

    if (!(format=elm_conf_read("Datetime", "TimeFormat"))) {
        format = "%-I:%M %p";
    }

    strftime(string, sizeof(string), format, tm);
    gtk_label_set_text(GTK_LABEL(*label), string);

    return TRUE;
}
