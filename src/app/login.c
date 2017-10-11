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
static GtkWidget * new_login_title(const char *text);
static GtkWidget * new_login_button(const char *text);
static int         set_callback_data(GtkWidget *widget, gpointer data);
static int         set_default_widget(GtkWidget *widget, gpointer data);

/* Private variables */
static ElmLoginInfo LoginInfo;
const  char *Style = "/etc/X11/elm/src/app/style/login.css";

/* ************************************************************************** */
/* Create login fields application */
GtkWidget * display_login(ElmCallback callback)
{
    elmprintf(LOG, "Displaying password entry box application...");

    static GtkWidget *container;
    static GtkWidget *entrybox;
    static GtkWidget *buttonbox;
    container = gtk_box_new(GTK_ORIENTATION_VERTICAL,   15);
    entrybox  = gtk_box_new(GTK_ORIENTATION_VERTICAL,    5);
    buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);


    static GtkWidget *title;
    static GtkWidget *username;
    static GtkWidget *password;
    static GtkWidget *xsession;
    static GtkWidget *button;
    title    = new_login_title("Login");
    username = new_username_widget();
    password = new_password_widget();
    xsession = new_xsession_widget();
    button   = new_login_button("Log in");

    gtk_box_pack_start(GTK_BOX(container), title,     FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(container), entrybox,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(container), buttonbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entrybox),  username,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entrybox),  password,  FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(buttonbox),   xsession,  FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(buttonbox),   button,    FALSE, FALSE, 0);

    g_signal_connect(button, "clicked", G_CALLBACK(callback), &LoginInfo);

    return container;
}

/* ************************************************************************** */
/* Create login title */
GtkWidget * new_login_title(const char *text)
{
    static GtkWidget *title;
    title = gtk_label_new(text);

    gtk_widget_set_halign(title, GTK_ALIGN_START);
    elm_set_widget_style(&title, "LoginTitle", Style);

    return title;
}

/* ************************************************************************** */
/* Create login button */
GtkWidget * new_login_button(const char *text)
{
    static GtkWidget *button;
    button = gtk_button_new_with_label(text);

    elm_set_widget_size(&button, 185, 0);
    elm_set_widget_style(&button, "LoginButton", Style);

    g_signal_connect(button, "clicked", G_CALLBACK(set_callback_data), NULL);
    g_signal_connect(button, "show",    G_CALLBACK(set_default_widget), NULL);

    return button;
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

/* ************************************************************************** */
/* Set button as the default widget when widget is shown (for sure is attached
 * to a window by then) */
int set_default_widget(GtkWidget *widget, gpointer data)
{
    printf("Setting default widget\n");
    GtkWidget *window = elm_get_window(&widget);
    elm_set_default_widget(&window, &widget);
    return 0;
}
