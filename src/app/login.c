/* *****************************************************************************
 * 
 * Name:    login.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display login fields for the Extensible Login Manager. This
 *              includes the title, username and password entry box, login
 *              button, and window manager menu.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "app/login.h"
#include "app/credentials.h"
#include "app/frame.h"
#include "app/xsession.h"
#include "elm.h"

/* Private functions */
static GtkWidget * new_login_button(const char *text);
static void        elm_app_set_default_widget(GtkWidget *widget, gpointer data);
static void        elm_app_set_focus_on_widget(GtkWidget *widget, gpointer data);

/* Private variables */
static const char *Style = "/etc/X11/elm/share/css/login.css";

/* ************************************************************************** */
/* Create login fields application */
GtkWidget * display_login(ElmCallback callback)
{
    elmprintf(LOGINFO, "Displaying password entry box application.");

    /* Create widgets */
    static GtkWidget *frame     = NULL;
    static GtkWidget *container = NULL;
    static GtkWidget *entrybox  = NULL;
    static GtkWidget *buttonbox = NULL;
    static GtkWidget *username  = NULL;
    static GtkWidget *password  = NULL;
    static GtkWidget *xsession  = NULL;
    static GtkWidget *button    = NULL;

    frame     = new_frame_widget();
    container = gtk_box_new(GTK_ORIENTATION_VERTICAL,   15);
    entrybox  = gtk_box_new(GTK_ORIENTATION_VERTICAL,    5);
    buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    username  = new_username_widget();
    password  = new_password_widget();
    xsession  = new_xsession_widget();
    button    = new_login_button("Login");

    /* Setup session information struct and its helpers */
    ElmSessionInfo       *info    = elm_session_info_new();
    ElmSessionInfoHelper *uhelper = elm_session_info_helper_new(username, info->username);
    ElmSessionInfoHelper *phelper = elm_session_info_helper_new(password, info->password);
    ElmSessionInfoHelper *xhelper = elm_session_info_helper_new(xsession, info->xsession);

    /* Finish setting up widgets */
    gtk_fixed_put(GTK_FIXED(frame), container, 0, 0);
    gtk_box_pack_start(GTK_BOX(container), entrybox,  TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(container), buttonbox, TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(entrybox),  username,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entrybox),  password,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttonbox), button,    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttonbox), xsession,  FALSE, FALSE, 0);
    gtk_widget_set_margin_top(container,   20);
    gtk_widget_set_margin_start(container, 20);

    g_signal_connect(button, "clicked", G_CALLBACK(set_credential_info),  uhelper);
    g_signal_connect(button, "clicked", G_CALLBACK(set_credential_info),  phelper);
    g_signal_connect(button, "clicked", G_CALLBACK(set_xsession_info),    xhelper);
    g_signal_connect(button, "clicked", G_CALLBACK(callback),             info);
    g_signal_connect(frame,  "show",    G_CALLBACK(elm_app_set_default_widget),  &button);
    g_signal_connect(frame,  "show",    G_CALLBACK(elm_app_set_focus_on_widget), &username);
    g_signal_connect(frame,  "map",     G_CALLBACK(elm_app_set_default_widget),  &button);
    g_signal_connect(frame,  "map",     G_CALLBACK(elm_app_set_focus_on_widget), &username);

    gtk_widget_show(username);
    gtk_widget_show(password);
    gtk_widget_show(xsession);
    gtk_widget_show(button);
    gtk_widget_show(entrybox);
    gtk_widget_show(buttonbox);
    gtk_widget_show(container);
    gtk_widget_show(frame);

    return frame;
}

/* ************************************************************************** */
/* Create login button */
GtkWidget * new_login_button(const char *text)
{
    static GtkWidget *button = NULL;

    button = gtk_button_new_with_label(text);

    elm_gtk_set_widget_size_from_conf(&button, "Login", "Width", "Height");
    elm_gtk_add_css_from_file(&button, "LoginButton", Style);

    return button;
}

/* ************************************************************************** */
/* Set button as the default widget when widget is shown */
void elm_app_set_default_widget(GtkWidget *widget, gpointer data)
{
    GtkWidget  *window = elm_gtk_get_window(&widget);
    GtkWidget **button = data;

    elm_gtk_default_widget(&window, button);
}

/* ************************************************************************** */
/* Set focus on widget */
void elm_app_set_focus_on_widget(GtkWidget *widget, gpointer data)
{
    GtkWidget **username = data;

    elm_gtk_focus(username);
}
