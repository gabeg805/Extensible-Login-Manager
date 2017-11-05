/* *****************************************************************************
 * 
 * Name:    xsession.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display button which allows a user to select which X Session to
 *              start after logging in.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "app/xsession.h"

/* Private functions */
static char ** get_available_xsessions(void);
static int     set_xsession_menu(GtkWidget **menu);

/* Private variables */
static const char      *Style   = "/etc/X11/elm/style/css/xsession.css";
static       GtkWidget *Xbutton = NULL;
static       GtkWidget *Xmenu   = NULL;

/* ************************************************************************** */
/* Create xsession menu button */
GtkWidget * new_xsession_widget(void)
{
    elmprintf(LOGINFO, "Displaying Xsession menu.");

    Xbutton = gtk_menu_button_new();
    Xmenu   = gtk_menu_new();

    set_xsession_menu(&Xmenu);
    gtk_button_set_relief(GTK_BUTTON(Xbutton), GTK_RELIEF_HALF);
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(Xbutton), Xmenu);
    elm_gtk_set_widget_size(&Xbutton, 30, 30);
    elm_gtk_set_widget_style(&Xbutton, "XSession", Style);
    gtk_widget_show_all(Xmenu);
    gtk_widget_show_all(Xbutton);

    return Xbutton;
}

/* ************************************************************************** */
/* Return xsession button widget */
GtkWidget * get_xsession_button_widget(void)
{
    return Xbutton;
}

/* ************************************************************************** */
/* Return xsession menu widget */
GtkWidget * get_xsession_menu_widget(void)
{
    return Xmenu;
}

/* ************************************************************************** */
/* Return xsession */
const char * get_xsession(void)
{
    GtkWidget   *menuitem = gtk_menu_get_active(GTK_MENU(Xmenu));
    const gchar *text     = gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));
    printf("WM: %s~\n", text);

    return text;
}

/* ************************************************************************** */
/* Return all avaiable xsessions on the system 
 * 
 * To-do: Read through file and search for Name= and Exec=. Display the Name=
 * strings and execute (with ElmLogin.xsession) the Exec= lines.
 */
char ** get_available_xsessions(void)
{
    DIR           *dstream  = opendir("/usr/share/xsessions/");
    const char    *ext      = ".desktop";
    size_t         extlen   = strlen(ext);
    struct dirent *entry;
    char          *filename;
    unsigned char  filetype;
    size_t         namelen;

    if (!dstream)
        return NULL;

    /* Iterate over files in directory */
    char   **xsessions = calloc(1, sizeof(filename));
    size_t   i         = 0;

    while ((entry=readdir(dstream)))
    {
        /* Check extension of file is correct */
        filename = entry->d_name;
        filetype = entry->d_type;
        if ((filetype != DT_REG) || (!strstr(filename, ext)))
            continue;

        /* Record filename without the extension */
        namelen      = strlen(filename) - extlen + 1;
        xsessions[i] = calloc(namelen, sizeof(*filename));
        if (xsessions[i])
            strncpy(xsessions[i], filename, namelen-1);
        else
            continue;

        /* Allocate space for next xsession */
        xsessions = realloc(xsessions, (i+2)*sizeof(filename));
        xsessions[++i] = NULL;
    }

    return xsessions;
}

/* ************************************************************************** */
/* Populate menu with xsession(s) on system */
int set_xsession_menu(GtkWidget **menu)
{
    char     **xsessions = get_available_xsessions();
    GSList    *group     = NULL;
    GtkWidget *menuitem  = NULL;
    size_t     i;

    if (!xsessions)
        return 1;

    /* Create the radio buttons for the window managers */
    for (i=0; xsessions[i]; i++)
    {
        menuitem = gtk_radio_menu_item_new_with_label(group, xsessions[i]);
        group    = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menuitem));

        gtk_menu_attach(GTK_MENU(*menu), menuitem, 0, 1, i, i+1);
        gtk_widget_show(menuitem);

        free(xsessions[i]);
        xsessions[i] = NULL;
    }

    free(xsessions);
    xsessions = NULL;

    return 0;
}
