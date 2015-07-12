/* *****************************************************************************
 * 
 * Name:    windowmanager.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Display the window manager button application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */
/* Includes */
#include "windowmanager.h"
#include "elyapp.h"
#include "elyconfig.h"
#include "elyglobal.h"
#include "elytype.h"
#include "utility.h"
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

/* Private functions */
static void setup_menu(struct elyapp *app);
static void wm_write_to_file(GtkMenu *item);
static char ** get_window_managers();
static void set_window_manager_entries(GtkWidget *menu);

/* **********************************
 * ***** DISPLAY WINDOW MANAGER *****
 * **********************************
 */
/* Display the window manager button */
void display_window_manager()
{
    TRACE(stdout, "%s", "Displaying window manager menu...");

    /* Define default window manager session */
    SESSION = read_config_char(WM_CONFIG, "wm", MAX_STR_LEN);
    if ( SESSION == NULL )
        SESSION = "xterm";

    /* Create the window manager menu application */
    static struct elyapp wm;
    wm.gui.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    wm.gui.widg = gtk_menu_button_new();
    setup_app_settings(&wm, WM_CONFIG, WM_STYLE, "WindowManager");
    set_app(&wm);
    set_app_style(&wm, wm.gui.win);
    set_app_style(&wm, wm.gui.widg);
    setup_menu(&wm);

    TRACE(stdout, "%s", "Done displaying window manager menu.");
}

/* *************************************
 * ***** SETUP WINDOW MANAGER MENU *****
 * *************************************
 */
/* Setup window manager dropdown menu button */
static void setup_menu(struct elyapp *app)
{
    TRACE(stdout, "%s", "Setting up window manager menu...");

    GtkWidget *menu = gtk_menu_new();
    set_window_manager_entries(menu);
    gtk_button_set_relief(GTK_BUTTON(app->gui.widg), GTK_RELIEF_HALF);
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(app->gui.widg), menu);

    TRACE(stdout, "%s", "Done setting up window manager menu.");
}

/* **************************************
 * ***** ADD WM ENTRIES TO THE MENU *****
 * **************************************
 */
/* Determine which window manager(s) the system has and add them as entries to the menu */
static void set_window_manager_entries(GtkWidget *menu)
{
    TRACE(stdout, "%s", "Creating window manager menu entries...");

    char **allwm  = get_window_managers();
    GSList *group = NULL;
    size_t i      = 0;
    GtkWidget *sesh;

    /* Create the radio buttons for the window managers */
    while ( allwm[i] != NULL ) {
        if ( i == 0 )
            sesh = gtk_radio_menu_item_new_with_label(0, allwm[i]);
        else
            sesh = gtk_radio_menu_item_new_with_label(group, allwm[i]);
        group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(sesh));
        gtk_menu_attach(GTK_MENU(menu), sesh, 0, 1, i, i+1);
        gtk_widget_show(sesh);
        g_signal_connect(G_OBJECT(sesh), "activate", G_CALLBACK(wm_write_to_file), NULL);

        free(allwm[i]);
        allwm[i] = NULL;
        ++i;
    }
    free(allwm);
    allwm = NULL;

    TRACE(stdout, "%s", "Done creating window manager entries.");
}

/* Return all the window managers on the system */
static char ** get_window_managers()
{
    TRACE(stdout, "%s", "Getting list of system window managers...");

    char *cmd    = read_config_char(WM_CONFIG, "cmd", MAX_CMD_LEN);
    char sep     = '\n';
    size_t count = 0;
    size_t loc   = 0;
    size_t i     = 0;
    size_t j     = 0;
    char arr[MAX_CMD_LEN];
    get_cmd_output(arr, sizeof(arr), cmd);

    /* Determine number of window managers on system */
    while ( arr[i] != 0 ) {
        if ( arr[i] == sep )
            ++count;
        ++i;
    }
    if ( count == 0 )
        return 0;

    i   = 0;
    j   = 0;
    loc = 0;
    char container[count+1][MAX_STR_LEN];
    memset(container[0], 0, MAX_STR_LEN);

    /* Create available window manager list */
    while ( arr[i] != 0 ) {
        if ( arr[i] != sep ) {
            container[loc][j] = arr[i];
            ++j;
        } else {
            j = 0;
            ++loc;
            memset(container[loc], 0, MAX_STR_LEN);
        }
        ++i;
    }

    char **ret = malloc(sizeof(char*) * count);
    bool add   = false;
    i   = 0;
    loc = 0;

    /* Add users to return array from available window manager list */
    while (1) {
        add = true;
        if ( (strcmp(SESSION, "xterm") == 0) ||
             (strcmp(container[loc], SESSION) == 0) ) 
            ;
        else if ( (i != 0) && (container[loc][0] != 0) ) 
            ;
        else
            add = false;

        if ( add == true ) {
            size_t size = strlen(container[loc]) + 1;
            ret[i] = malloc(size);
            snprintf(ret[i], size, "%s", container[loc]);
            container[loc][0] = 0;
            ++i;
            if ( i == count )
                break;
        }
        ++loc;
        if ( loc >= count )
            loc = 0;
    }
    ret[i] = 0;

    TRACE(stdout, "%s", "Done getting list of system window managers.");

    return ret;
}

/* ****************************************
 * ***** WRITE WINDOW MANAGER TO FILE *****
 * ****************************************
 */
/* Write to a file, which window manager to use for the session */
static void wm_write_to_file(GtkMenu *item)
{
    TRACE(stdout, "%s", "Setting window manager session...");

    SESSION = (char *) gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    overwrite_config_line(WM_CONFIG, "wm", SESSION);

    TRACE(stdout, "%s", "Done setting window manager session.");
}
