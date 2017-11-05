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
static gboolean set_date(gpointer data);
static gboolean set_time(gpointer data);

/* Private variables */
static const char *Style      = "/etc/X11/elm/style/css/datetime.css";
static const char *DateFormat = "%A, %B %-d";
static const char *TimeFormat = "%-I:%M %p";

/* ************************************************************************** */
/* Create date and time application */
GtkWidget * display_datetime(ElmCallback callback)
{
    elmprintf(LOGINFO, "Displaying date and time.");

    static GtkWidget *box;
    static GtkWidget *date;
    static GtkWidget *time;

    box  = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    date = gtk_label_new("");
    time = gtk_label_new("");

    gtk_box_pack_start(GTK_BOX(box), time, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), date, TRUE, TRUE, 0);
    gtk_widget_set_halign(date, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(time, GTK_ALIGN_CENTER);
    elm_gtk_set_widget_style(&date, "Date", Style);
    elm_gtk_set_widget_style(&time, "Time", Style);
    set_date(&date);
    set_time(&time);
    g_timeout_add_seconds(5, set_date, &date);
    g_timeout_add_seconds(5, set_time, &time);
    gtk_widget_show(date);
    gtk_widget_show(time);
    gtk_widget_show(box);

    return box;
}

/* ************************************************************************** */
/* Set date */
gboolean set_date(gpointer data)
{
    GtkWidget **label = (GtkWidget**) data;
    time_t      now   = time(NULL);
    struct tm  *tm    = localtime(&now);
    char        string[64];

    strftime(string, sizeof(string), DateFormat, tm);
    gtk_label_set_text(GTK_LABEL(*label), string);

    return TRUE;
}

/* ************************************************************************** */
/* Set time */
gboolean set_time(gpointer data)
{
    GtkWidget **label = (GtkWidget**) data;
    time_t      now   = time(NULL);
    struct tm  *tm    = localtime(&now);
    char        string[64];

    strftime(string, sizeof(string), TimeFormat, tm);
    gtk_label_set_text(GTK_LABEL(*label), string);

    return TRUE;
}
