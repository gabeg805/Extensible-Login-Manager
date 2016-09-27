/* *****************************************************************************
 * 
 * Name:    datetime.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display the date and time for the Extensible Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "app/datetime.h"
#include "elm.h"

/* Private functions */
static gboolean set_date_and_time(gpointer data);

/* Private variables */
static const char *Style = "/etc/X11/elm/src/app/style/datetime.css";

/* ************************************************************************** */
/* Create date and time application */
ElmApp * display_datetime(ElmCallback callback)
{
    elmprintf(LOG, "Displaying date and time...");
    static ElmApp *datetime;
    datetime = elm_new_app();
    datetime->new_window_with_style(GTK_WINDOW_POPUP, "Window", Style);
    datetime->set_position(110, 595);
    set_date_and_time(datetime);
    g_timeout_add_seconds(20, set_date_and_time, datetime);
    return datetime;
}

/* ************************************************************************** */
/* Set date and time */
gboolean set_date_and_time(gpointer data)
{
    ElmApp *datetime = (ElmApp*)data;
    static GtkWidget *box       = NULL;
    static GtkWidget *datelabel = NULL;
    static GtkWidget *timelabel = NULL;

    /* Initialize widgets */
    if (datetime->get_length() == 0) {
        box       = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        datelabel = gtk_label_new("");
        timelabel = gtk_label_new("");
        gtk_label_set_xalign(GTK_LABEL(timelabel), 0);
        gtk_label_set_xalign(GTK_LABEL(datelabel), 0);
        gtk_box_pack_start(GTK_BOX(box), timelabel, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(box), datelabel, TRUE, TRUE, 0);
        datetime->set_self(datetime);
        datetime->add_container(box);
        datetime->add_widget(timelabel);
        datetime->add_widget(datelabel);
        datetime->set_widget_style("Time", timelabel, Style);
        datetime->set_widget_style("Date", datelabel, Style);
    }

    /* Set widget labels */
    time_t     now = time(NULL);
    struct tm *tm  = localtime(&now);
    char datestr[64];
    char timestr[64];
    strftime(datestr, sizeof(datestr), "%A, %B %-d", tm);
    strftime(timestr, sizeof(timestr), "%-I:%M %p",  tm);
    gtk_label_set_text(GTK_LABEL(datelabel), datestr);
    gtk_label_set_text(GTK_LABEL(timelabel), timestr);
    return TRUE;
}
