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

/* Typedefs */
typedef enum
{
    NONE     = 0,
    INFO     = (1 <<  0) * 1,
    INFNO    = (1 <<  0) * 2,
    WARN     = (1 <<  2) * 1,
    WARNO    = (1 <<  2) * 2,
    ERROR    = (1 <<  4) * 1,
    ERRNO    = (1 <<  4) * 2,
    LOGINFO  = (1 <<  6) * 1,
    LOGINFNO = (1 <<  6) * 2,
    LOGWARN  = (1 <<  8) * 1,
    LOGWARNO = (1 <<  8) * 2,
    LOGERR   = (1 << 10) * 1,
    LOGERRNO = (1 << 10) * 2
} ElmPrint;

/* Public functions  */
void elmprintf(ElmPrint mode, const char *vafmt, ...);
void elm_io_set_verbose(int flag);

#endif /* ELM_IO_H */
