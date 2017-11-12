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
static int      elm_app_set_xsession_menu(GtkWidget **menu);
static char *** elm_app_get_available_xsessions(void);

/* Private variables */
static const char *Style = "/etc/X11/elm/share/css/xsession.css";

/* ************************************************************************** */
/* Create xsession menu button */
GtkWidget * new_xsession_widget(void)
{
    elmprintf(LOGINFO, "Displaying Xsession menu.");

    /* Create widgets */
    static GtkWidget *Xbutton = NULL;
    static GtkWidget *Xmenu   = NULL;

    Xbutton = gtk_menu_button_new();
    Xmenu   = gtk_menu_new();

    /* Setup widgets */
    elm_app_set_xsession_menu(&Xmenu);
    elm_gtk_set_widget_size_from_conf(&Xbutton, "XSession", "Width", "Height");
    elm_gtk_add_css_from_file(&Xbutton, "XSession", Style);
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(Xbutton), Xmenu);

    gtk_widget_show_all(Xmenu);
    gtk_widget_show_all(Xbutton);

    return Xbutton;
}

/* ************************************************************************** */
/* Set xsession information */
void set_xsession_info(GtkWidget *widget, gpointer data)
{
    ElmSessionInfoHelper  *helper = data;
    GtkMenuButton         *button = GTK_MENU_BUTTON(helper->widget);
    GtkMenu               *menu   = gtk_menu_button_get_popup(button);
    GtkMenuItem           *item   = GTK_MENU_ITEM(gtk_menu_get_active(menu));
    const gchar           *text   = gtk_widget_get_tooltip_text(GTK_WIDGET(item));
    size_t                 length = strlen(text)+1;

    printf("Info : %s~\n", text);

    memset(helper->data, 0, length);
    strncpy(helper->data, text, length);
}

/* ************************************************************************** */
/* Populate menu with xsession(s) on system */
int elm_app_set_xsession_menu(GtkWidget **menu)
{
    char      ***xsessions = elm_app_get_available_xsessions();
    GSList      *group     = NULL;
    GtkWidget   *menuitem  = NULL;
    size_t       index;

    if (!xsessions) {
        return -1;
    }

    /* Create the radio buttons for the window managers */
    for (index=0; xsessions[0][index] && xsessions[1][index]; index++)
    {
        menuitem = gtk_radio_menu_item_new_with_label(group, xsessions[0][index]);
        group    = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menuitem));

        gtk_widget_set_tooltip_text(menuitem, xsessions[1][index]);
        gtk_menu_attach(GTK_MENU(*menu), menuitem, 0, 1, index, index+1);
        gtk_widget_show(menuitem);

        elm_std_free(&xsessions[0][index]);
        elm_std_free(&xsessions[1][index]);
    }

    /* Clear memory */
    elm_std_free(&xsessions[0]);
    elm_std_free(&xsessions[1]);
    elm_std_free(&xsessions);

    return 0;
}

/* ************************************************************************** */
/* Return all avaiable xsessions on the system */
char *** elm_app_get_available_xsessions(void)
{
    /* Open directory for reading */
    const char    *dir     = "/usr/share/xsessions";
    DIR           *dhandle = opendir(dir);
    struct dirent *entry;

    if (!dhandle) {
        return NULL;
    }

    /* Allocate initial memory spaces for main pointer and its two columns */
    char   ***xsessions = NULL;
    char     *path      = NULL;
    char     *nameline  = NULL;
    char     *execline  = NULL;
    size_t    index     = 1;
    size_t    i;

    if (!(xsessions=calloc(2, sizeof *xsessions))) {
        elmprintf(LOGERRNO, "Unable to allocate xsessions array");
        return NULL;
    }
    else {
        xsessions[0] = NULL;
        xsessions[1] = NULL;
    }

    if (!(xsessions[0]=calloc(1, sizeof *xsessions[0]))) {
        elmprintf(LOGERRNO, "%s '%lu'",
                  "Unable to allocate xsessions array", 0);
        goto cleanup;
    }

    if (!(xsessions[1]=calloc(1, sizeof *xsessions[0]))) {
        elmprintf(LOGERRNO, "%s '%lu'",
                  "Unable to allocate xsessions array", 1);
        goto cleanup;
    }

    /* Iterate over files in directory */
    while ((entry=readdir(dhandle)))
    {
        /* Clear previously allocated memory */
        if (path) {
            elm_std_free(&path);
        }

        if (nameline) {
            elm_std_free(&nameline);
        }

        if (execline) {
            elm_std_free(&execline);
        }

        /* Check extension of file is correct */
        if ((entry->d_type != DT_REG) || (!strstr(entry->d_name, ".desktop"))) {
            continue;
        }

        /* Read file */
        path     = elm_str_path("%s/%s", dir, entry->d_name);
        nameline = elm_str_findline(path, "Name=");
        execline = elm_str_findline(path, "Exec=");

        if (!nameline || !execline) {
            continue;
        }

        /* Copy string to array */
        if (!(xsessions[0][index-1]=elm_str_copy(&nameline[5]))) {
            goto cleanup;
        }

        if (!(xsessions[1][index-1]=elm_str_copy(&execline[5]))) {
            goto cleanup;
        }

        /* Resize allocated memory for next xsession */
        if (!(xsessions[0]=realloc(xsessions[0],
                                   (index+1) * sizeof *xsessions[0])))
        {
            elmprintf(LOGERRNO, "Unable to reallocate xsessions array");
            goto cleanup;
        }

        if (!(xsessions[1]=realloc(xsessions[1],
                                   (index+1) * sizeof *xsessions[0])))
        {
            elmprintf(LOGERRNO, "Unable to reallocate xsessions array");
            goto cleanup;
        }

        xsessions[0][index] = NULL;
        xsessions[1][index] = NULL;
        index++;
    }

    closedir(dhandle);

    return xsessions;

cleanup:
    if (path) {
        elm_std_free(&path);
    }

    if (nameline) {
        elm_std_free(&nameline);
    }

    if (execline) {
        elm_std_free(&execline);
    }

    for (i=0; i < (index-1); i++) {
        elm_std_free(&xsessions[0][i]);
        elm_std_free(&xsessions[1][i]);
    }

    if (xsessions[0]) {
        elm_std_free(&xsessions[0]);
    }
    if (xsessions[1]) {
        elm_std_free(&xsessions[1]);
    }

    elm_std_free(&xsessions);

    return NULL;
}
