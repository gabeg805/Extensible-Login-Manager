/* *****************************************************************************
 * 
 * Name:    credentials.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display username and password entries.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "app/credentials.h"

/* Private functions */
static int elm_app_set_entry_buffer(GtkWidget *widget, char *placeholder);
static int elm_app_set_entry_icon(GtkWidget *widget, char *name);
static int elm_app_set_default_user(GtkWidget *widget);

/* Private variables */
static const char *Style = "/etc/X11/elm/share/css/credentials.css";

/* ************************************************************************** */
/* Create username entry */
GtkWidget * new_username_widget(void)
{
    elmprintf(LOGINFO, "Displaying username entry.");

    /* Create widget */
    static GtkWidget *Username = NULL;

    Username = gtk_entry_new();

    /* Setup widget */
    elm_app_set_entry_buffer(Username, "Username");
    elm_app_set_entry_icon(Username, "Username");
    elm_app_set_default_user(Username);
    elm_gtk_set_widget_size_from_conf(&Username, "Credentials", "Width", "Height");
    elm_gtk_add_css_from_file(&Username, NULL, Style);
    gtk_entry_set_activates_default(GTK_ENTRY(Username), TRUE);

    gtk_widget_show(Username);

    return Username;
}

/* ************************************************************************** */
/* Create password entry */
GtkWidget * new_password_widget(void)
{
    elmprintf(LOGINFO, "Displaying password entry.");

    /* Create widget */
    static GtkWidget *Password = NULL;

    Password = gtk_entry_new();

    /* Setup widget */
    elm_app_set_entry_buffer(Password, "Password");
    elm_app_set_entry_icon(Password, "Password");
    gtk_entry_set_visibility(GTK_ENTRY(Password), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(Password), '*');
    gtk_entry_set_activates_default(GTK_ENTRY(Password), TRUE);
    elm_gtk_set_widget_size_from_conf(&Password, "Credentials", "Width", "Height");
    elm_gtk_add_css_from_file(&Password, NULL, Style);

    gtk_widget_show(Password);

    return Password;
}

/* ************************************************************************** */
/* Set username/password credential information */
void set_credential_info(GtkWidget *widget, gpointer data)
{
    ElmSessionInfoHelper *helper = data;
    GtkEntry             *entry  = GTK_ENTRY(helper->widget);
    GtkEntryBuffer       *buffer = gtk_entry_get_buffer(entry);
    const gchar          *text   = gtk_entry_buffer_get_text(buffer);
    size_t                length = strlen(text)+1;

    printf("Info : %s~\n", text);

    elm_app_set_entry_buffer(helper->widget, NULL);
    memset(helper->data, 0, length);
    strncpy(helper->data, text, length);
}

/* ************************************************************************** */
/* Set entry box text buffer */
int elm_app_set_entry_buffer(GtkWidget *widget, char *placeholder)
{
    static const int  length = 32;
    GtkEntryBuffer   *buffer = gtk_entry_buffer_new(NULL, -1);

    gtk_entry_buffer_set_max_length(buffer, length);
    gtk_entry_set_buffer(GTK_ENTRY(widget), buffer);

    if (placeholder) {
        gtk_entry_set_placeholder_text(GTK_ENTRY(widget), placeholder);
    }

    return 0;
}

/* ************************************************************************** */
/* Set entry box icon, if specified in config file */
int elm_app_set_entry_icon(GtkWidget *widget, char *name)
{
    char      *icon;
    GdkPixbuf *pixbuf;

    if (!(icon=elm_conf_read("Images", name))) {
        return -1;
    }

    pixbuf = gdk_pixbuf_new_from_file(icon, NULL);

    gtk_entry_set_icon_from_pixbuf(GTK_ENTRY(widget), GTK_ENTRY_ICON_PRIMARY,
                                   pixbuf);

    return 0;
}

/* ************************************************************************** */
/* Set default user, if specified in config file */
int elm_app_set_default_user(GtkWidget *widget)
{
    char *user = NULL;

    if (!(user=elm_conf_read("Main", "DefaultUser")) || !user[0]) {
        return -1;
    }

    gtk_entry_set_text(GTK_ENTRY(widget), user);

    return 0;
}
