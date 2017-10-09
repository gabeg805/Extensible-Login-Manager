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
#include "app/xsession.h"
#include "elm.h"

/* Private functions */
static int new_login_container(ElmApp *login);
static int new_login_title(const char *text, ElmApp *login);
static int new_login_credential(const char *type, ElmApp *login);
static int new_login_xsession(ElmApp *login);
static int new_login_button(const char *text, ElmApp *login);
static int set_callback_data(GtkWidget *widget, gpointer data);

/* Private variables */
static ElmLoginInfo LoginInfo;
const  char *Style = "/etc/X11/elm/src/app/style/login.css";

/* ************************************************************************** */
/* Create login fields application */
ElmApp * display_login(ElmCallback callback)
{
    elmprintf(LOG, "Displaying password entry box application...");

    static ElmApp *login;
    login = elm_new_app();
    login->new_window_with_style(GTK_WINDOW_TOPLEVEL, "Window", Style);
    login->set_position(570, 277);
    login->set_callback(callback);

    new_login_container(login);
    new_login_title("Login", login);
    new_login_credential("Username", login);
    new_login_credential("Password", login);
    new_login_xsession(login);
    new_login_button("Log in", login);

    return login;
}

/* ************************************************************************** */
/* Create container for login widgets */
int new_login_container(ElmApp *login)
{
    static GtkWidget *container;
    static GtkWidget *entrybox;
    static GtkWidget *buttonbox;
    container = gtk_box_new(GTK_ORIENTATION_VERTICAL,   15);
    entrybox  = gtk_box_new(GTK_ORIENTATION_VERTICAL,    5);
    buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    gtk_box_pack_end(GTK_BOX(container), buttonbox, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(container), entrybox,  FALSE, FALSE, 0);

    int status;
    if ((status=login->add_container(container)) != 0)
        return status;
    if ((status=login->add_widget(entrybox)) != 0)
        return status;
    if ((status=login->add_widget(buttonbox)) != 0)
        return status;

    return 0;
}

/* ************************************************************************** */
/* Create login title */
int new_login_title(const char *text, ElmApp *login)
{
    static GtkWidget *title;
    GtkWidget        *loginbox;
    loginbox = login->get_widgets()[0];
    title    = gtk_label_new(text);

    gtk_label_set_xalign(GTK_LABEL(title), 0);
    gtk_box_pack_start(GTK_BOX(loginbox), title, FALSE, FALSE, 0);

    int status;
    if ((status=login->add_widget(title)) != 0)
        return status;
    if ((status=login->set_widget_style("Title", title, Style)) != 0)
        return status;

    return 0;
}

/* ************************************************************************** */
/* Create login credentials */
int new_login_credential(const char *type, ElmApp *login)
{
    GtkWidget *entrybox = login->get_widgets()[1];
    GtkWidget *widget;

    if (strcmp(type, "Username") == 0)
        widget = new_username_widget();
    else if (strcmp(type, "Password") == 0)
        widget = new_password_widget();
    else
        return 1;

    gtk_box_pack_start(GTK_BOX(entrybox), widget, FALSE, FALSE, 0);

    int status;
    if ((status=login->add_widget(widget)) != 0)
        return status;
    if ((status=login->set_widget_size(widget, 230, 0)) != 0)
        return status;
    if ((status=login->set_widget_style(type, widget, Style)) != 0)
        return status;

    return 0;
}

/* ************************************************************************** */
/* Create login xsession button */
int new_login_xsession(ElmApp *login)
{
    static GtkWidget *xbutton;
    static GtkWidget *xmenu;
    GtkWidget        *buttonbox;
    xbutton   = new_xsession_widget();
    xmenu     = get_xsession_menu_widget();
    buttonbox = login->get_widgets()[2];

    gtk_box_pack_end(GTK_BOX(buttonbox), xbutton, FALSE, FALSE, 0);

    int status;
    if ((status=login->add_widget(xmenu)) != 0 )
        return status;
    if ((status=login->set_widget_style("WindowManager", xbutton, Style))
        != 0)
        return status;

    return 0;
}

/* ************************************************************************** */
/* Create login button */
int new_login_button(const char *text, ElmApp *login)
{
    static GtkWidget *button;
    GtkWidget        *buttonbox;
    button    = gtk_button_new_with_label(text);
    buttonbox = login->get_widgets()[2];

    gtk_box_pack_end(GTK_BOX(buttonbox), button, FALSE, FALSE, 0);

    int status;
    if ((status=login->add_widget(button)) != 0)
        return status;
    if ((status=login->set_widget_size(button, 185, 0)) != 0)
        return status;
    if ((status=login->set_default_widget(button)) != 0)
        return status;
    if ((status=login->set_widget_style("Login", button, Style)) != 0)
        return status;

    g_signal_connect(button, "clicked", G_CALLBACK(set_callback_data), NULL);
    g_signal_connect(button, "clicked", G_CALLBACK(login->get_callback()),
                     &LoginInfo);

    return 0;
}

/* ************************************************************************** */
/* Set data to be handed off to callback */
int set_callback_data(GtkWidget *widget, gpointer data)
{
    const char *username = get_username();
    const char *password = get_password();
    const char *xsession = get_xsession();
    size_t      ulen     = strlen(username)+1;
    size_t      plen     = strlen(password)+1;
    size_t      xlen     = strlen(xsession)+1;

    LoginInfo.username = calloc(ulen, sizeof(*username));
    LoginInfo.password = calloc(plen, sizeof(*password));
    LoginInfo.xsession = calloc(xlen, sizeof(*xsession));

    snprintf(LoginInfo.username, ulen, username);
    snprintf(LoginInfo.password, plen, password);
    snprintf(LoginInfo.xsession, xlen, xsession);

    return 0;
}
