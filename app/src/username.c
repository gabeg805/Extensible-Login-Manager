/* *****************************************************************************
 * 
 * Name:    username.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Display the username dropdown menu application which displays 
 *              all users that are able to login to the system.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "username.h"
#include "elyglobal.h"
#include "elyapp.h"
#include "elytype.h"
#include "elyconfig.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <gtk/gtk.h>

/* Declares */
static void setup_menu(struct elyapp app);
static void setup_label(GtkWidget *label, struct elytxt txt);
static void usermenu_write_to_file(GtkMenu *item, GtkWidget *label);
static char ** get_usernames(char *file);
static void set_username_entries(GtkWidget *menu, GtkWidget *label);
static void display_icon();
static void display_usr_menu();



/* ******************************* */
/* ***** SETUP USERNAME MENU ***** */
/* ******************************* */

/* Setup dropdown menu that displays users */
static void setup_menu(struct elyapp app) {
    TRACE(stdout, "%s", "Setting up username menu...");

    GtkWidget *label = gtk_label_new("");
    GtkWidget *menu  = gtk_menu_new();

    set_username_entries(menu, label);
    setup_label(label, app.txt);
    gtk_menu_button_set_popup(GTK_MENU_BUTTON(app.widg), menu);
    gtk_container_add(GTK_CONTAINER(app.widg), label);
    gtk_button_set_relief(GTK_BUTTON(app.widg), GTK_RELIEF_NORMAL);
    gtk_widget_show(label);

    TRACE(stdout, "%s", "Done setting up menu.");
}



/* Setup the label for the dropdown menu */
static void setup_label(GtkWidget *label, struct elytxt txt) {
    TRACE(stdout, "%s", "Setting up username menu labels...");

    PangoAttrList *attrList   = pango_attr_list_new();
    PangoAttribute *attrFont  = pango_attr_family_new(txt.font);
    PangoAttribute *attrSize  = pango_attr_size_new((long)1024 * txt.size);
    PangoAttribute *attrColor = pango_attr_foreground_new((long)256 * txt.red,
                                                          (long)256 * txt.green,
                                                          (long)256 * txt.blue);

    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrColor);
    gtk_label_set_text(GTK_LABEL(label), USERNAME);
    gtk_label_set_attributes(GTK_LABEL(label), attrList);

    TRACE(stdout, "%s", "Done setting up labels.");
}



/* **************************************** */
/* ***** WRITE USER MENU ITEM TO FILE ***** */
/* **************************************** */

/* Write to a file, which user to login as */
static void usermenu_write_to_file(GtkMenu *item, GtkWidget *label) {
    TRACE(stdout, "%s", "Setting username...");

    USERNAME = (char*) gtk_menu_item_get_label(GTK_MENU_ITEM(item));
    overwrite_config_line(USER_CONFIG, "user", USERNAME);
    gtk_label_set_text(GTK_LABEL(label), USERNAME);

    TRACE(stdout, "%s", "Done setting username.");
}



/* *********************************** */
/* ***** GET ALL USERS ON SYSTEM ***** */
/* *********************************** */

/* Get username*uid combination from the specified file */
static char ** get_usernames(char *file) {
    TRACE(stdout, "%s", "Getting list of users on system...");

    FILE *handle  = fopen(file, "r");
    size_t count  = 0;
    char sep      = ':';
    char line[MAX_CMD_LEN];
    char container[MAX_NUM_LEN][MAX_STR_LEN];
    char uid_str[MAX_NUM_LEN];
    char user[MAX_STR_LEN];

    /* Add user (based on UID) to the avaiable user list */
    while ( fgets(line, sizeof(line), handle) != 0 ) {
        get_substring(uid_str, line, sep, 3);
        int uid = atoi(uid_str);
        if ( (uid == 0) || ((uid >= 1000) && (uid <= 9999)) ) {
            get_substring(user, line, sep, 1);
            snprintf(container[count], strlen(user)+1, "%s", user);
            ++count;
        }
    }
    
    /* No users found */
    if ( count == 0 )
        return 0;
    
    /* Allocate necessary space to hold all users */
    char **ret = malloc(sizeof(char*) * count);
    size_t i   = 0;
    size_t loc = 0;
    bool add   = false;

    /* Add users to return array from available user list */
    while (1) {
        add = true;
        if ( (strcmp(USERNAME, "User") == 0) ||
             (strcmp(container[loc], USERNAME) == 0) ) 
            ;
        else if ( (i != 0) && (container[loc][0] != 0) ) 
            ;
        else
            add = false;

        /* Setup the menu items */
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
    fclose(handle);

    TRACE(stdout, "%s", "Done getting list of system users.");

    return ret;
}



/* ******************************************** */
/* ***** ADD USERNAME ENTRIES TO THE MENU ***** */
/* ******************************************** */

/* Set user name entries */
static void set_username_entries(GtkWidget *menu, GtkWidget *label) {
    TRACE(stdout, "%s", "Creating username menu entries...");

    GSList *group   = 0;
    char *files[]   = {"/etc/passwd", "/etc/shadow", 0};
    char **allusers = get_usernames(files[0]);
    GtkWidget *user;
    size_t i = 0;

    if ( allusers == 0 )
        allusers = get_usernames(files[1]);

    /* Create a menu item for each user on the system */
    while ( allusers[i] != 0 ) {
        if ( i == 0 )
            user = gtk_radio_menu_item_new_with_label(NULL, allusers[i]);
        else
            user = gtk_radio_menu_item_new_with_label(group, allusers[i]);

        group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(user));
        gtk_menu_attach(GTK_MENU(menu), user, 0, 1, i, i+1);
        gtk_widget_show(user);
        g_signal_connect(G_OBJECT(user), "activate", G_CALLBACK(usermenu_write_to_file), label);

        free(allusers[i]);
        allusers[i] = 0;
        ++i;
    }
    free(allusers);
    allusers = 0;

    TRACE(stdout, "%s", "Done creating username entries.");
}



/* **************************** */
/* ***** DISPLAY USERNAME ***** */
/* **************************** */

/* Display username icon */
static void display_icon() {
    TRACE(stdout, "%s", "Displaying username icon...");

    /* Create the icon widget  */
    struct elyapp app;
    app.win  = gtk_window_new(GTK_WINDOW_POPUP);
    app.widg = gtk_image_new();
    setup_app(USER_IMG_CONFIG, &app, 0, 0);
    gtk_image_set_from_file(GTK_IMAGE(app.widg), app.decor.img_file);

    TRACE(stdout, "%s", "Done displaying username icon.");
}



/* Display the username menu */
static void display_usr_menu() {
    TRACE(stdout, "%s", "Displaying username menu application...");
    
    /* Define default username */
    USERNAME = read_config_char(USER_CONFIG, "user", MAX_STR_LEN);
    if ( USERNAME == 0 ) 
        USERNAME = "User";

    /* Create the username menu widget */
    struct elyapp app;
    app.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    app.widg = gtk_menu_button_new();
    setup_app(USER_CONFIG, &app, NULL, NULL);
    setup_menu(app);

    TRACE(stdout, "%s", "Done displaying username menu application.");
}



/* Display username and icon widgets */
void display_username() {
    display_icon();
    display_usr_menu();
}
