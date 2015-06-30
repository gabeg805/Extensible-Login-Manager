/* *****************************************************************************
 * 
 * Name:    password.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Display the password entry box application.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "password.h"
#include "elyglobal.h"
#include "elyapp.h"
#include "elytype.h"
#include "utility.h"
#include <gtk/gtk.h>

/* Private functions */
static void setup_entry();
static void get_entry_text(GtkWidget *widg);

/* Application (if error, could be cuz of static) */
static struct elyapp APP;



/* *************************** */
/* ***** SETUP ENTRY BOX ***** */
/* *************************** */

/* Setup the entry box */
static void setup_entry() {
    TRACE(stdout, "%s", "Setting up password entry box...");

    gtk_entry_set_visibility(GTK_ENTRY(APP.widg), FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(APP.widg), APP.txt.invis[0]);
    GtkEntryBuffer *buf = gtk_entry_buffer_new(0, -1);
    gtk_entry_buffer_set_max_length(buf, APP.txt.maxchars);
    gtk_entry_set_buffer(GTK_ENTRY(APP.widg), buf);
    PangoAttrList *attrList   = pango_attr_list_new();
    PangoAttribute *attrFont  = pango_attr_family_new(APP.txt.font);
    PangoAttribute *attrSize  = pango_attr_size_new((long)1024 * APP.txt.size);
    PangoAttribute *attrColor = pango_attr_foreground_new((long)256 * APP.txt.red, 
                                                          (long)256 * APP.txt.green, 
                                                          (long)256 * APP.txt.blue);
    attrList = pango_attr_list_ref(attrList);
    attrList = pango_attr_list_ref(attrList);
    pango_attr_list_insert(attrList, attrFont);
    pango_attr_list_insert(attrList, attrSize);
    pango_attr_list_insert(attrList, attrColor);
    gtk_entry_set_attributes(GTK_ENTRY(APP.widg), attrList);

    TRACE(stdout, "%s", "Done setting up password entry box.");
}



/* ************************** */
/* ***** GET ENTRY TEXT ***** */
/* ************************** */

/* Return user entry text */
static void get_entry_text(GtkWidget *widg) {
    TRACE(stdout, "%s", "Getting password entry box text...");

    GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY(APP.widg));
    const gchar *text   = gtk_entry_buffer_get_text(buf);
    guint len           = gtk_entry_buffer_get_length(buf);
    buf                 = gtk_entry_buffer_new(0, -1);
    gtk_entry_buffer_set_max_length(buf, APP.txt.maxchars);
    gtk_entry_set_buffer(GTK_ENTRY(APP.widg), buf);
    if ( len != 0 ) {
        gtk_widget_destroy(APP.widg);
        gtk_main_quit();
        PASSWORD = (char*) text;
    }

    TRACE(stdout, "%s", "Done getting password text.");
} 



/* ************************************** */
/* ***** DISPLAY PASSWORD ENTRY BOX ***** */
/* ************************************** */

/* Display the password entry box */
void display_password_entry() {
    TRACE(stdout, "%s", "Displaying password entry box application...");

    /* Create the password entry box */
    APP.win  = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    APP.widg = gtk_entry_new();
    setup_app(PASS_CONFIG, &APP, "activate", get_entry_text);
    setup_entry();

    TRACE(stdout, "%s", "Done displaying password entry box.");
}
