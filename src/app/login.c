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
static void        set_callback_data(GtkWidget *widget, gpointer data);
static void        set_default_widget(GtkWidget *widget, gpointer data);
static void        set_focus_on_widget(GtkWidget *widget, gpointer data);

/* Private variables */
static const char      *Style = "/etc/X11/elm/style/css/login.css";
static       ElmLogin   Info;

/* ************************************************************************** */
/* Create login fields application */
GtkWidget * display_login(ElmCallback callback)
{
    elmprintf(LOGINFO, "Displaying password entry box application.");

    static GtkWidget *frame;
    static GtkWidget *container;
    static GtkWidget *entrybox;
    static GtkWidget *buttonbox;
    static GtkWidget *username;
    static GtkWidget *password;
    static GtkWidget *xsession;
    static GtkWidget *button;

    frame     = new_frame_widget();
    container = gtk_box_new(GTK_ORIENTATION_VERTICAL,   15);
    entrybox  = gtk_box_new(GTK_ORIENTATION_VERTICAL,    5);
    buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    username  = new_username_widget();
    password  = new_password_widget();
    xsession  = new_xsession_widget();
    button    = new_login_button("Login");

    gtk_fixed_put(GTK_FIXED(frame), container, 0, 0);
    gtk_box_pack_start(GTK_BOX(container), entrybox,  TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(container), buttonbox, TRUE,  TRUE,  0);
    gtk_box_pack_start(GTK_BOX(entrybox),  username,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entrybox),  password,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttonbox), button,    FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(buttonbox), xsession,  FALSE, FALSE, 0);
    gtk_widget_set_margin_top(container,   20);
    gtk_widget_set_margin_start(container, 20);
    g_signal_connect(button, "clicked", G_CALLBACK(callback),            &Info);
    g_signal_connect(frame,  "show",    G_CALLBACK(set_default_widget),  &button);
    g_signal_connect(frame,  "show",    G_CALLBACK(set_focus_on_widget), &username);
    g_signal_connect(frame,  "map",     G_CALLBACK(set_default_widget),  &button);
    g_signal_connect(frame,  "map",     G_CALLBACK(set_focus_on_widget), &username);
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
    static GtkWidget *button;

    button = gtk_button_new_with_label(text);

    elm_gtk_set_widget_size(&button, 185, 30);
    elm_gtk_set_widget_style(&button, "LoginButton", Style);

    g_signal_connect(button, "clicked", G_CALLBACK(set_callback_data), NULL);

    return button;
}

/* ************************************************************************** */
/* Set data to be handed off to callback 
 * 
 * To-do: If size of username/password is greater than set array size, logging
 * in will always fail. Similarly, for xsession, starting the X session will
 * always fail. 
 */
void set_callback_data(GtkWidget *widget, gpointer data)
{
    const char *username = get_username();
    const char *password = get_password();
    const char *xsession = get_xsession();

    memset(Info.username, 0, sizeof(Info.username));
    memset(Info.password, 0, sizeof(Info.password));
    memset(Info.xsession, 0, sizeof(Info.xsession));

    strncpy(Info.username, username, sizeof(Info.username)-1);
    strncpy(Info.password, password, sizeof(Info.password)-1);
    strncpy(Info.xsession, xsession, sizeof(Info.xsession)-1);
}

/* ************************************************************************** */
/* Set button as the default widget when widget is shown */
void set_default_widget(GtkWidget *widget, gpointer data)
{
    GtkWidget  *window = elm_gtk_get_window(&widget);
    GtkWidget **button = data;

    elm_gtk_set_default_widget(&window, button);
}

/* ************************************************************************** */
/* Set focus on widget */
void set_focus_on_widget(GtkWidget *widget, gpointer data)
{
    GtkWidget **username = data;

    gtk_widget_grab_focus(*username);
}
