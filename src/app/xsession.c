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
static int     set_xsession_menu(GtkWidget **menu);
static char ** get_available_xsessions(void);

/* Private variables */
static const char      *Style   = "/etc/X11/elm/share/css/xsession.css";
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
int set_xsession_menu(GtkWidget **menu)
{
    char      **xsessions = NULL;
    GSList     *group     = NULL;
    GtkWidget  *menuitem  = NULL;
    size_t      index     = 0;

    /* Create the radio buttons for the window managers */
    for (index=0; (xsessions=get_available_xsessions()); index++)
    {
        menuitem = gtk_radio_menu_item_new_with_label(group, xsessions[0]);
        group    = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(menuitem));

        gtk_widget_set_tooltip_text(menuitem, xsessions[1]);
        gtk_menu_attach(GTK_MENU(*menu), menuitem, 0, 1, index, index+1);
        gtk_widget_show(menuitem);
    }

    if (index == 0) {
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/* Return all avaiable xsessions on the system */
char ** get_available_xsessions(void)
{
    static char   ***xsessions = NULL;
    static char     *pair[2]   = {0};
    static size_t    index     = 0;

    /* Return next session pair in list */
    if (xsessions) {
        free(xsessions[0][index]);
        free(xsessions[1][index++]);

        pair[0] = xsessions[0][index];
        pair[1] = xsessions[1][index];

        if (pair[0] && pair[1]) {
            return pair;
        }
        else {
            goto maincleanup;
        }
    }

    /* Open directory for reading */
    const char    *dir     = "/usr/share/xsessions";
    DIR           *dhandle = opendir(dir);
    FILE          *fhandle;
    struct dirent *entry;

    if (!dhandle) {
        return NULL;
    }

    /* Prepare variables */
    size_t  namesize = 1;
    size_t  execsize = 1;
    size_t  row;
    size_t  col;
    char   *pos;
    char   *path;
    char    line[ELM_MAX_LINE_SIZE];

    /* Iterate over files in directory */
    while ((entry=readdir(dhandle)))
    {
        /* Check extension of file is correct */
        if ((entry->d_type != DT_REG) || (!strstr(entry->d_name, ".desktop"))) {
            continue;
        }

        /* Prepare to read file */
        path = elm_sys_path("%s/%s", dir, entry->d_name);

        if (!(fhandle=fopen(path, "r"))) {
            continue;
        }

        /* Read file */
        while (fgets(line, sizeof(line), fhandle))
        {
            if (line[4] != '=') {
                continue;
            }

            /* Remove newline */
            if ((pos=strchr(line, '\n'))) {
                *pos = 0;
            }

            /* Find name and exec command */
            if (strstr(line, "Name=")) {
                row = 0;
                col = namesize;
                namesize++;
            }
            else if (strstr(line, "Exec=")) {
                row = 1;
                col = execsize;
                execsize++;
            }
            else {
                continue;
            }

            /* Allocate memory if not already allocated */
            if (!elm_calloc(&xsessions, 2, sizeof *xsessions)) {
                elmprintf(LOGERRNO, "Unable to allocate xsessions array");
                goto cleanup;
            }

            if (!elm_calloc(&xsessions[row], 1, sizeof *xsessions[0])) {
                elmprintf(LOGERRNO, "%s '%lu'",
                          "Unable to allocate xsessions array", row);
                goto cleanup;
            }

            /* Save string to return */
            if (!elm_string_copy(&xsessions[row][col-1], &line[5])) {
                goto cleanup;
            }

            /* Resize allocated memory for next xsession */
            if (!elm_realloc(&xsessions[row], col+1, sizeof *xsessions[0])) {
                elmprintf(LOGERRNO, "Unable to reallocate xsessions array");
                goto cleanup;
            }

            xsessions[row][col] = NULL;
        }

        fclose(fhandle);
    }

    closedir(dhandle);

    pair[0] = xsessions[0][0];
    pair[1] = xsessions[1][0];

    return pair;

maincleanup:
    free(xsessions[0]);
    free(xsessions[1]);

    xsessions[0] = NULL;
    xsessions[1] = NULL;

cleanup:
    free(xsessions);

    xsessions    = NULL;
    pair[0]      = NULL;
    pair[1]      = NULL;
    index        = 0;

    return NULL;
}
