/* *****************************************************************************
 * 
 * Name:    elmio.h
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: ELM input/output utilities.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Header guard */
#ifndef ELM_IO_H
#define ELM_IO_H

/* Inlcudes */
#include <errno.h>
#include <string.h>

/* Defines */
#define ELM_LOG "/tmp/elm.log"

/* Typedefs */
typedef enum { NONE, INFO, WARNING, ERROR, LOG, DEBUG } ElmPrint_t;

/* Public functions  */
void elmprintf(ElmPrint_t mode, const char *fmt, ...);
void elm_set_verbose(int flag);

#endif /* ELM_IO_H */
