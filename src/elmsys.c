/* *****************************************************************************
 * 
 * Name:    elmsys.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Common system functions.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmsys.h"
#include "elmdef.h"
#include "elmio.h"
#include "elmstr.h"
#include <elmstd.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/* ************************************************************************** */
/* Search for a process that matches the input name and return its PID */
pid_t elm_sys_pgrep(const char *program)
{
    uid_t         uid   = getuid();
    pid_t         pid   = 0;
    char         *ptr;
    char         *end;
    char         *fpath;
    char          buffer[ELM_MAX_MSG_SIZE];
    struct stat   info;
    int           nbytes;
    int           fd;

    /* Search for a process that matches the input name */
    char **proc = elm_sys_get_proc();
    size_t i;

    for (i=0; proc[i]; i++)
    {
        /* Focus on freeing memory once pid is set */
        if (pid > 0) {
            continue;
        }

        /* Make sure user spawned process */
        fpath = elm_str_path("/proc/%s/cmdline", proc[i]);

        if ((stat(fpath, &info) < 0) \
            || ((info.st_uid != uid) && (uid > 0)) \
            || (access(fpath, R_OK) < 0))
        {
            elm_std_free(&fpath);
            elm_std_free(&proc[i]);
            continue;
        }

        /* Read cmdline file for pid */
        if (((fd=open(fpath, O_RDONLY)) < 0) \
            || ((nbytes=read(fd, buffer, sizeof(buffer))) <= 0))
        {
            elm_std_free(&fpath);
            elm_std_free(&proc[i]);
            continue;
        }

        /* Set pid if program is found in cmdline */
        end = buffer + nbytes;
        for (ptr=buffer; ptr < end; ) {
            if (strstr(ptr, program)) {
                pid = strtol(proc[i], 0, 10);
                break;
            }

            while (*ptr++) {}
        }

        close(fd);
        elm_std_free(&fpath);
        elm_std_free(&proc[i]);
    }

    return pid;
}

/* ************************************************************************** */
/* Return the basename of a path*/
char * elm_sys_basename(const char *string)
{
    size_t      size = strlen(string)+1;
    static char buf[ELM_MAX_PATH_SIZE];

    size = (sizeof(buf) < size) ? sizeof(buf)-1 : size;

    memset(buf,0, size);
    memcpy(buf, string, size);

    /* Find path basename */
    char   *ret   = buf;
    size_t  shift = 0;
    size_t  i;

    for (i=0; ret[i] != '\0'; i++) {
        shift = (ret[i] == '/') ? i+1 : shift;
    }

    ret += shift;

    return ret;
}

/* ************************************************************************** */
/* Look to see if name corresponds to a running process. Return PID if it
 * does. */
char ** elm_sys_get_proc(void)
{
    DIR           *dhandle = opendir("/proc");
    struct dirent *entry;
    char          *endptr;

    if (!dhandle) {
        return NULL;
    }

    /* Populate process list with current processes */
    char   **procs = NULL;
    size_t   index = 1;
    size_t   i;

    while ((entry=readdir(dhandle)))
    {
        /* Check process conditions */
        if (entry->d_type != DT_DIR) {
            continue;
        }

        if (!strtol(entry->d_name, &endptr, 10) || (*endptr != '\0')) {
            continue;
        }

        /* Allocate memory for process list */
        if (!procs) {
            if (!(procs=calloc(1, sizeof *procs))) {
                elmprintf(LOGERRNO, "Unable to allocate process array");
                goto cleanup;
            }
        }

        /* Copy string to process list */
        if (!(procs[index-1]=elm_str_copy(entry->d_name))) {
            goto cleanup;
        }

        /* Increase size of allocated memory region */
        if (!(procs=realloc(procs, (index+1) * sizeof *procs))) {
            elmprintf(LOGERRNO, "Unable to reallocate process array");
            goto cleanup;
        }

        procs[index++] = NULL;
    }

    closedir(dhandle);

    return procs;

cleanup:
    for (i=0; i < index; i++) {
        elm_std_free(&procs[i]);
    }

    elm_std_free(&procs);

    return NULL;
}
