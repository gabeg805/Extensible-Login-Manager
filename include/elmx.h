/* *****************************************************************************
 * 
 * Name:    elmx.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Setup the X server for the Extensible Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_X_H
#define ELM_X_H

/* Defines */
#define ELM_EXIT_X_START    10
#define ELM_EXIT_X_OPEN     11
#define ELM_EXIT_X_WAIT     12
#define ELM_EXIT_X_STOP     13
#define ELM_EXIT_X_DISPLAY  14
#define ELM_EXIT_X_TTY      15
#define ELM_EXIT_X_XCOMPMGR 16

/* Public functions */
int xstart(void);
int xcompmanager(void);
int xsetstyle(void);
int xinitrc(void);

#endif /* ELM_X_H */
