/* *****************************************************************************
 * 
 * Name:    elminterface.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Interface between the creation of the applications and the
 *              Extensible Login Manager.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_INTERFACE_H
#define ELM_INTERFACE_H

/* Includes */
#include "elmapp.h"
#include "elmgtk.h"

/* Public functions */
ElmApp * login_interface(void);

#endif /* ELM_INTERFACE_H */
