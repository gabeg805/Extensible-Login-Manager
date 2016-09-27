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
#include "elm.h"

/* Private functions */
static int set_widget_container(ElmApp *login);
static int set_title_label(const char *text, ElmApp *login);
static int set_username_entry(const char *text, ElmApp *login);
static int set_password_entry(const char *text, ElmApp *login);
static int set_login_button(const char *text, ElmApp *login);
static int set_window_manager_menu(ElmApp *login);
static int set_window_manager_menu_items(GtkWidget *menu);
static int set_entry_buffer(GtkWidget *widget, char *placeholder);
static int set_callback_data(GtkWidget *widget, gpointer data);
static void get_username(GtkWidget *button, gpointer data);
static void get_password(GtkWidget *button, gpointer data);
static void get_window_manager(GtkWidget *button, gpointer data);
static const gchar * get_entry_buffer_text(GtkWidget *widget, char **field);
static char ** get_available_window_managers(void);

/* Private variables */
static ElmLoginInfo LoginInfo;
static char Username[64];
static char Password[64];
static char WindowManager[64];
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
    set_widget_container(login);
    set_title_label("Login", login);
    set_username_entry("Username", login);
    set_password_entry("Password", login);
    set_window_manager_menu(login);
    set_login_button("Log in", login);
    return login;
}

/* ************************************************************************** */
/* Create widget container */
int set_widget_container(ElmApp *login)
{
    static GtkWidget *container, *entrybox, *buttonbox;
    container = gtk_box_new(GTK_ORIENTATION_VERTICAL,   15);
    entrybox  = gtk_box_new(GTK_ORIENTATION_VERTICAL,    5);
    buttonbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_end(GTK_BOX(container), buttonbox, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(container), entrybox,  FALSE, FALSE, 0);
    int ret;
    if ((ret=login->add_container(container)) != 0)
        return ret;
    if ((ret=login->add_widget(entrybox)) != 0)
        return ret;
    if ((ret=login->add_widget(buttonbox)) != 0)
        return ret;
    return 0;
}

/* ************************************************************************** */
/* Create title label */
int set_title_label(const char *text, ElmApp *login)
{
    static GtkWidget *title;
    title = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(title), 0);
    GtkWidget *container = login->get_widgets()[0];
    gtk_box_pack_start(GTK_BOX(container), title, FALSE, FALSE, 0);
    int ret;
    if ((ret=login->add_widget(title)) != 0)
        return ret;
    if ((ret=login->set_widget_style("Title", title, Style)) != 0)
        return ret;
    return 0;
}

/* ************************************************************************** */
/* Create username entry */
int set_username_entry(const char *text, ElmApp *login)
{
    static GtkWidget *username;
    username = gtk_entry_new();
    set_entry_buffer(username, (char*)text);
    gtk_entry_set_activates_default(GTK_ENTRY(username), TRUE);
    GtkWidget *entrybox = login->get_widgets()[1];
    gtk_box_pack_start(GTK_BOX(entrybox), username, FALSE, FALSE, 0);
    int ret;
    if ((ret=login->add_widget(username)) != 0)
        return ret;
    if ((ret=login->set_widget_size(username, 230, 0)) != 0)
        return ret;
    if ((ret=login->set_widget_style("Username", username, Style)) != 0)
        return ret;
    return 0;
}

/* ************************************************************************** */
/* Create password entry */
int set_password_entry(const char *text, ElmApp *login)
{
    static GtkWidget *password;
    password = gtk_entry_new();
    set_entry_buffer(password, (char*)text);
    gtk_entry_set_visibility(GTK_ENTRY(password), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(password), '*');
    gtk_entry_set_activates_default(GTK_ENTRY(password), TRUE);
    GtkWidget *entrybox = login->get_widgets()[1];
    gtk_box_pack_start(GTK_BOX(entrybox), password, FALSE, FALSE, 0);
    int ret;
    if ((ret=login->add_widget(password)) != 0)
        return ret;
    if ((ret=login->set_widget_size(password, 230, 0)) != 0)
        return ret;
    if ((ret=login->set_widget_style("Password", password, Style)) != 0)
        return ret;
    return 0;
}

/* ************************************************************************** */
/* Create login button */
int set_login_button(const char *text, ElmApp *login)
{
    static GtkWidget *button;
    button = gtk_button_new_with_label(text);
    GtkWidget *buttonbox = login->get_widgets()[2];
    gtk_box_pack_end(GTK_BOX(buttonbox), button, FALSE, FALSE, 0);
    int ret;
    if ((ret=login->add_widget(button)) != 0)
        return ret;
    if ((ret=login->set_widget_size(button, 185, 0)) != 0)
        return ret;
    if ((ret=login->set_default_widget(button)) != 0)
        return ret;
    if ((ret=login->set_widget_style("Login", button, Style)) != 0)
        return ret;
    g_signal_connect(button, "clicked", G_CALLBACK(get_username), login);
    g_signal_connect(button, "clicked", G_CALLBACK(get_password), login);
    g_signal_connect(button, "clicked", G_CALLBACK(get_window_manager), login);
    g_signal_connect(button, "clicked", G_CALLBACK(set_callback_data), NULL);
    g_signal_connect(button, "clicked", G_CALLBACK(login->get_callback()),
                     &LoginInfo);
    return 0;
}

/* ************************************************************************** */
/* Create window manager menu button */
int set_window_manager_menu(ElmApp *login)
{
    static GtkWidget *wmbutton, *wmmenu;
    wmbutton = gtk_menu_button_new();
    wmmenu   = gtk_menu_new();
    set_window_manager_menu_items(wmmenu);
    gtk_button_set_relief(GTK_BUTTON(wmbutton), GTK_RELIEF_HALF);
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(wmbutton), wmmenu);
    GtkWidget *buttonbox = login->get_widgets()[2];
    gtk_box_pack_end(GTK_BOX(buttonbox), wmbutton, FALSE, FALSE, 0);
    int ret;
    if ((ret=login->add_widget(wmmenu)) != 0 )
        return ret;
    if ((ret=login->set_widget_style("WindowManager", wmbutton, Style))
        != 0)
        return ret;
    return 0;
}

/* ************************************************************************** */
/* Determine available window manager(s) on system */
int set_window_manager_menu_items(GtkWidget *menu)
{
    char **windowmanagers = get_available_window_managers();
    if (windowmanagers == NULL)
        return 1;
    GSList *group = NULL;
    size_t  i     = 0;
    GtkWidget *menuitem;

    /* Create the radio buttons for the window managers */
    while ( windowmanagers[i] != NULL ) {
        menuitem = gtk_radio_menu_item_new_with_label(group, windowmanagers[i]);
        group    = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menuitem));
        gtk_menu_attach(GTK_MENU(menu), menuitem, 0, 1, i, i+1);
        gtk_widget_show(menuitem);
        free(windowmanagers[i]);
        windowmanagers[i] = NULL;
        ++i;
    }
    free(windowmanagers);
    windowmanagers = NULL;
    return 0;
}

/* ************************************************************************** */
/* Set entry box text buffer */
int set_entry_buffer(GtkWidget *widget, char *placeholder)
{
    GtkEntryBuffer *buf = gtk_entry_buffer_new(0, -1);
    gtk_entry_buffer_set_max_length(buf, 32);
    gtk_entry_set_buffer(GTK_ENTRY(widget), buf);
    if (placeholder != NULL)
        gtk_entry_set_placeholder_text(GTK_ENTRY(widget), placeholder);
    return 0;
}

/* ************************************************************************** */
/* Set data to be handed off to callback */
int set_callback_data(GtkWidget *widget, gpointer data)
{
    LoginInfo.username      = Username;
    LoginInfo.password      = Password;
    LoginInfo.windowmanager = WindowManager;
    return 0;
}

/* ************************************************************************** */
/* Get username */
void get_username(GtkWidget *button, gpointer data)
{
    ElmApp      *login    = (ElmApp*)data;
    GtkWidget   *username = login->get_widgets()[4];
    const gchar *buffer   = get_entry_buffer_text(username, NULL);
    snprintf(Username, sizeof(Username), "%s", buffer);
    printf("Username: %s~\n", Username);
}

/* ************************************************************************** */
/* Get password */
void get_password(GtkWidget *button, gpointer data)
{
    ElmApp      *login    = (ElmApp*)data;
    GtkWidget   *password = login->get_widgets()[5];
    const gchar *buffer   = get_entry_buffer_text(password, NULL);
    snprintf(Password, sizeof(Password), "%s", buffer);
    printf("Password: %s~\n", Password);
}

/* ************************************************************************** */
/* Get window manager */
void get_window_manager(GtkWidget *button, gpointer data)
{
    ElmApp    *login    = (ElmApp*)data;
    GtkWidget *wmmenu   = login->get_widgets()[6];
    GtkWidget *menuitem = gtk_menu_get_active(GTK_MENU(wmmenu));
    const gchar *label  = gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));
    snprintf(WindowManager, sizeof(WindowManager), "%s", label);
    printf("WM: %s~\n", WindowManager);
}

/* ************************************************************************** */
/* Get entry box text */
const gchar * get_entry_buffer_text(GtkWidget *widget, char **field)
{
    GtkEntryBuffer *buf  = gtk_entry_get_buffer(GTK_ENTRY(widget));
    const gchar    *text = gtk_entry_buffer_get_text(buf);
    set_entry_buffer(widget, NULL);
    return text;
}

/* ************************************************************************** */
/* Return all the window managers on the system */
char ** get_available_window_managers(void)
{
    DIR  *dstream = opendir("/usr/share/xsessions/");
    if (dstream == NULL)
        return NULL;
    struct dirent *entry;
    char          *name;
    unsigned char  type;

    /* Iterate over files in directory */
    size_t   count = 0;
    size_t   length;
    char    *last;
    char   **windowmanagers = (char**)calloc(1, sizeof(name));
    windowmanagers[0] = NULL;
    while ((entry=readdir(dstream)) != NULL) {
        type = entry->d_type;
        name = entry->d_name;
        if ((type != DT_REG) || (strstr(name, ".desktop") == NULL))
            continue;
        length = strlen(name);
        windowmanagers[count] = (char*)calloc(length+1, sizeof(*name));
        if (windowmanagers[count] == NULL)
            continue;
        strncpy(windowmanagers[count], name, length);
        if ((last=strrchr(windowmanagers[count], '.')) != NULL)
            *last = '\0';
        ++count;
        windowmanagers = (char**)realloc(windowmanagers,
                                         (count+1)*sizeof(name));
        windowmanagers[count] = NULL;
    }
    return windowmanagers;
}
