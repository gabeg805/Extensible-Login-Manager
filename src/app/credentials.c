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
static int set_entry_buffer(GtkWidget *widget, char *placeholder);

/* Private variables */
static const char       *Style        = "/etc/X11/elm/style/css/credentials.css";
static const char       *UserIcon     = "/etc/X11/elm/style/icons/user.png";
static const char       *PasswordIcon = "/etc/X11/elm/style/icons/password.png";
static const int         EntryLength  = 32;
static       GtkWidget  *Username     = NULL;
static       GtkWidget  *Password     = NULL;

/* ************************************************************************** */
/* Create username entry */
GtkWidget * new_username_widget(void)
{
    elmprintf(LOGINFO, "Displaying username entry.");

    GdkPixbuf *pixbuf;

    Username = gtk_entry_new();
    pixbuf   = gdk_pixbuf_new_from_file(UserIcon, NULL);

    gtk_entry_set_icon_from_pixbuf(GTK_ENTRY(Username), GTK_ENTRY_ICON_PRIMARY,
                                   pixbuf);
    set_entry_buffer(Username, "Username");
    gtk_widget_set_can_focus(Username, TRUE);
    gtk_entry_set_activates_default(GTK_ENTRY(Username), TRUE);
    elm_gtk_set_widget_size(&Username, 230, 0);
    elm_gtk_set_widget_style(&Username, "Username", Style);
    gtk_widget_show(Username);

    return Username;
}

/* ************************************************************************** */
/* Create password entry */
GtkWidget * new_password_widget(void)
{
    elmprintf(LOGINFO, "Displaying password entry.");

    GdkPixbuf *pixbuf;

    Password = gtk_entry_new();
    pixbuf   = gdk_pixbuf_new_from_file(PasswordIcon, NULL);

    gtk_entry_set_icon_from_pixbuf(GTK_ENTRY(Password), GTK_ENTRY_ICON_PRIMARY,
                                   pixbuf);
    set_entry_buffer(Password, "Password");
    gtk_entry_set_visibility(GTK_ENTRY(Password), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(Password), '*');
    gtk_entry_set_activates_default(GTK_ENTRY(Password), TRUE);
    elm_gtk_set_widget_size(&Password, 230, 0);
    elm_gtk_set_widget_style(&Password, "Password", Style);
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

    set_entry_buffer(helper->widget, NULL);
    memset(helper->data, 0, length);
    strncpy(helper->data, text, length);
}

/* ************************************************************************** */
/* Set entry box text buffer */
int set_entry_buffer(GtkWidget *widget, char *placeholder)
{
    GtkEntryBuffer *buffer = gtk_entry_buffer_new(NULL, -1);

    gtk_entry_buffer_set_max_length(buffer, EntryLength);
    gtk_entry_set_buffer(GTK_ENTRY(widget), buffer);

    if (placeholder) {
        gtk_entry_set_placeholder_text(GTK_ENTRY(widget), placeholder);
    }

    return 0;
}
