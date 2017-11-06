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
        /* Clear previous memory */
        free(xsessions[0][index]);
        free(xsessions[1][index++]);

        pair[0] = xsessions[0][index];
        pair[1] = xsessions[1][index];

        if (pair[0] && pair[1]) {
            return pair;
        }
        else {
            /* Clear all allocated memory */
            free(xsessions[0]);
            free(xsessions[1]);

            xsessions[0] = NULL;
            xsessions[1] = NULL;

            free(xsessions);

            xsessions    = NULL;
            pair[0]      = NULL;
            pair[1]      = NULL;
            index        = 0;

            return NULL;
        }
    }

    /* Open directory for reading */
    const char *dir     = "/usr/share/xsessions";
    DIR        *dhandle = opendir(dir);

    if (!dhandle) {
        return NULL;
    }

    /* Prepare variables */
    size_t         namesize = 1;
    size_t         execsize = 1;
    size_t         row;
    size_t         col;
    size_t         length;
    FILE          *fhandle;
    char          *filename;
    char          *pos;
    char          *substring;
    char           line[128];
    char           path[128];
    struct dirent *entry;

    /* Iterate over files in directory */
    while ((entry=readdir(dhandle)))
    {
        /* Check extension of file is correct */
        filename = entry->d_name;

        if ((entry->d_type != DT_REG) || (!strstr(entry->d_name, ".desktop"))) {
            continue;
        }

        /* Prepare to read file */
        snprintf(path, sizeof(path), "%s/%s", dir, filename);

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
                col = namesize-1;
                namesize++;
            }
            else if (strstr(line, "Exec=")) {
                row = 1;
                col = execsize-1;
                execsize++;
            }
            else {
                continue;
            }

            /* Allocate memory if not already allocated */
            if (!xsessions) {
                xsessions = calloc(2, sizeof(*xsessions));

                if (!xsessions) {
                    printf("Unable to allocate main xsessions pointer");
                    return NULL;
                }
            }

            if (!xsessions[row]) {
                xsessions[row] = calloc(1, sizeof(*xsessions[0]));

                if (!xsessions[row]) {
                    printf("Unable to allocate xsessions row '%lu'", row);
                    return NULL;
                }
            }

            /* Save string to return */
            substring           = &line[5];
            length              = strlen(substring)+1;
            xsessions[row][col] = calloc(length, sizeof(*substring));

            if (!xsessions[row][col]) {
                printf("Unable to allocate xsessions row/col (%lu,%lu)", row, col);
                return NULL;
            }

            strncpy(xsessions[row][col], substring, length);
            xsessions[row] = realloc(xsessions[row], (col+2)*sizeof(*xsessions[0]));

            if (!xsessions[row]) {
                printf("Unable to reallocate xsessions");
                return NULL;
            }
        }

        fclose(fhandle);
    }

    closedir(dhandle);

    pair[0] = xsessions[0][0];
    pair[1] = xsessions[1][0];

    return pair;
}
