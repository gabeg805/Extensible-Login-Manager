/* *****************************************************************************
 * 
 * Name:    login.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Display the username and password entry box applications.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */
/* Includes */
#include "login.h"
#include "elyapp.h"
#include "elyglobal.h"
#include "elytype.h"
#include "utility.h"
#include <gtk/gtk.h>

/* Private functions */
static void setup_entry(struct elyapp *app, char *tag);
static void get_entry_text(struct elyapp *app, char **str);
static void get_username(void);
static void get_password(void);
static void get_login(GtkWidget *entry);

/* Application (if error, could be cuz of static) */
static struct elyapp user;
static struct elyapp pass;
static struct elyapp signin;

/* ********************************
 * ***** DISPLAY LOGIN FIELDS *****
 * ********************************
 */
void display_login()
{
    TRACE(stdout, "%s", "Displaying password entry box application...");

    /* Create the password entry box */
    user.gui.win    = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    pass.gui.win    = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    signin.gui.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    user.gui.widg   = gtk_entry_new();
    pass.gui.widg   = gtk_entry_new();
    signin.gui.widg = gtk_button_new_with_label("Log in");
    setup_app_settings(&user,   USER_CONFIG,   USER_STYLE,   "Username");
    setup_app_settings(&pass,   PASS_CONFIG,   PASS_STYLE,   "Password");
    setup_app_settings(&signin, SIGNIN_CONFIG, SIGNIN_STYLE, "Signin");
    set_app(&user);
    set_app(&pass);
    set_app(&signin);
    g_signal_connect(user.gui.widg,   "activate", G_CALLBACK(get_login), NULL);
    g_signal_connect(pass.gui.widg,   "activate", G_CALLBACK(get_login), NULL);
    g_signal_connect(signin.gui.widg, "clicked",  G_CALLBACK(get_login), NULL);

    setup_entry(&user, user.settings.class);
    setup_entry(&pass, pass.settings.class);
    set_app_style(&user,   user.gui.widg);
    set_app_style(&pass,   pass.gui.widg);
    set_app_style(&signin, signin.gui.widg);
    set_app_style(&user,   user.gui.win);
    set_app_style(&pass,   pass.gui.win);
    set_app_style(&signin, signin.gui.win);

    TRACE(stdout, "%s", "Done displaying password entry box.");
}

/* ************************
 * ***** SETUP ENTRY BOX **
 * ************************
 */
static void setup_entry(struct elyapp *app, char *tag)
{
    TRACE(stdout, "Setting up %s entry box...", tag);

    if ( app->text.invis != NULL ) {
        gtk_entry_set_visibility(GTK_ENTRY(app->gui.widg),
                                 FALSE);
        gtk_entry_set_invisible_char(GTK_ENTRY(app->gui.widg),
                                     *app->text.invis);
    }
    GtkEntryBuffer *buf = gtk_entry_buffer_new(0, -1);
    gtk_entry_buffer_set_max_length(buf, app->text.maxchars);
    gtk_entry_set_buffer(GTK_ENTRY(app->gui.widg), buf);
    gtk_entry_set_placeholder_text(GTK_ENTRY(app->gui.widg), tag);

    TRACE(stdout, "Done setting up %s entry box.", tag);
}

/* **************************
 * ***** GET LOGIN INFO *****
 * **************************
 */
/* Get username and password login information from the entry boxes */
static void get_login(GtkWidget *entry)
{
    TRACE(stdout, "%s", "Getting login text...");
    get_username();
    get_password();
    gtk_main_quit();
    TRACE(stdout, "%s", "Done getting login text.");
}

/* Get username entry box text */
static void get_username(void)
{
    TRACE(stdout, "%s", "Getting username entry box text...");
    get_entry_text(&user, &USERNAME);
    TRACE(stdout, "%s", "Done getting username text.");
} 

/* Get password entry box text */
static void get_password(void)
{
    TRACE(stdout, "%s", "Getting password entry box text...");
    get_entry_text(&pass, &PASSWORD);
    TRACE(stdout, "%s", "Done getting password text.");
} 

/* Get entry box text */
static void get_entry_text(struct elyapp *app, char **str)
{
    GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY(app->gui.widg));
    const gchar *text   = gtk_entry_buffer_get_text(buf);
    printf("~%s~\n", text);
    buf                 = gtk_entry_buffer_new(0, -1);
    gtk_entry_buffer_set_max_length(buf, app->text.maxchars);
    gtk_entry_set_buffer(GTK_ENTRY(app->gui.widg), buf);
    gtk_widget_destroy(app->gui.widg);
    *str = (char*)text;
}
