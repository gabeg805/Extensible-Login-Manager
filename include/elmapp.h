/* *****************************************************************************
 * 
 * Name:    elmapp.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Application builder, in the form of C-Style class, for the
 *              Extensible Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_APP_H
#define ELM_APP_H

/* Includes */
#include <stdint.h>
#include <gtk/gtk.h>

/* Typedefs */
typedef int (*ElmCallback)(GtkWidget*,void*);

typedef struct ElmApp
{
    int (*set_self)(struct ElmApp*);
    int (*new_window)(GtkWindowType);
    int (*new_window_with_style)(GtkWindowType, const char*, const char*);
    int (*add_container)(GtkWidget*);
    int (*add_widget)(GtkWidget*);
    int (*set_default_widget)(GtkWidget*);
    int (*set_length)(uint8_t);
    int (*set_position)(int16_t, int16_t);
    int (*set_size)(size_t, size_t);
    int (*set_widget_size)(GtkWidget*, size_t, size_t);
    int (*set_widget_style)(const char*, GtkWidget*, const char*);
    int (*set_callback)(ElmCallback);
    int (*show_all)(void);
    int (*hide_all)(void);
    uint8_t      (*get_length)(void);
    GtkWidget *  (*get_default_widget)(void);
    GtkWidget *  (*get_window)(void);
    GtkWidget ** (*get_widgets)(void);
    ElmCallback  (*get_callback)(void);

    GtkWidget    *_window;
    GtkWidget   **_widgets;
    ElmCallback   _callback;
    uint8_t       _length;
} ElmApp;

typedef ElmApp * (*ElmAppBuilder)(ElmCallback);

/* Public functions */
ElmApp * elm_new_app(void);

#endif /* ELM_APP_H */
