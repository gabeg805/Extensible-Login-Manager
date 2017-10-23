/* *****************************************************************************
 * 
 * Name:    elmx.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Setup the X server.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_X_H
#define ELM_X_H

/* Public functions */
int elm_x_run(void);
int elm_x_set_cursor(void);
int elm_x_set_transparency(int flag);
int elm_x_load_user_preferences(void);
int elm_x_set_screen_dimensions(void);
int elm_x_get_screen_width(void);
int elm_x_get_screen_height(void);

#endif /* ELM_X_H */
