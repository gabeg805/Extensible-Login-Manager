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
    NONE,
    INFO,
    WARNING,
    ERROR,
    LOG,
    LOGINFO,
    LOGWARN,
    LOGERR,
} ElmPrint;

/* Public functions  */
void elmprintf(ElmPrint mode, const char *vafmt, ...);
void elm_io_set_verbose(int flag);

#endif /* ELM_IO_H */
