/* *****************************************************************************
 * 
 * Name:    datetime.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display the date and time.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_DATETIME_H
#define ELM_DATETIME_H

/* Includes */
#include "elmapp.h"

/* Public functions */
GtkWidget * display_datetime(ElmCallback callback);

#endif /* ELM_DATETIME_H */
