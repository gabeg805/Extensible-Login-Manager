/* *****************************************************************************
 * 
 * Name:    utility.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Common functions used by ELM.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "utility.h"
#include "elmdef.h"
#include "elmio.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* ************************************************************************** */
/* Execute a command/file */
int elm_sys_exec(char *file, char *const argv[])
{
    elmprintf(LOGINFO, "%s '%s'.", "Running", file);
    execvp(file, argv);
    elmprintf(LOGERRNO, "%s '%s'", "Error trying to run", file);

    return -1;
}

/* ************************************************************************** */
/* Set environment variables */
int elm_setenv(char *name, char *value)
{
    elmprintf(LOGINFO, "Setting environment variable: '%s=%s'.", name, value);

    if (setenv(name, value, 1) < 0) {
        elmprintf(LOGERR, "%s '%s=%s': %s.",
                  "Unable to set environment variable", name, value,
                  strerror(errno));
        return 1;
    }

    return 0;
}

/* ************************************************************************** */
/* Return the basename of a path*/
char * elm_sys_basename(const char *string)
{
    size_t      size = strlen(string)+1;
    static char buf[ELM_MAX_PATH_SIZE];

    size = (sizeof(buf) < size) ? sizeof(buf)-1 : size;

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
/* Search for a process that matches the input name and return its PID */
pid_t elm_sys_pgrep(const char *program)
{
    char        **procs = elm_sys_get_proc();
    uid_t         uid   = getuid();
    pid_t         pid   = 0;
    struct stat   info;
    char         *ptr;
    char         *end;
    char          fpath[ELM_MAX_PATH_SIZE];
    char          buffer[ELM_MAX_MSG_SIZE];
    int           nbytes;
    int           fd;
    int           i;

    /* Search for a process that matches the input name */
    for (i=0; procs[i]; free(procs[i]), i++)
    {
        /* Focus on freeing memory once pid is set */
        if (pid > 0) {
            continue;
        }

        /* Make sure user spawned process */
        snprintf(fpath, sizeof(fpath), "/proc/%s/cmdline", procs[i]);

        if (   (stat(fpath, &info) < 0) \
            || ((info.st_uid != uid) && (uid > 0)) \
            || (access(fpath, R_OK) < 0))
        {
            continue;
        }

        /* Read cmdline file for pid */
        if (   ((fd=open(fpath, O_RDONLY)) < 0) \
            || ((nbytes=read(fd, buffer, sizeof(buffer))) <= 0))
        {
            continue;
        }

        /* Set pid if program is found in cmdline */
        end = buffer + nbytes;
        for (ptr=buffer; ptr < end; ) {
            if (strstr(ptr, program)) {
                pid = strtol(procs[i], 0, 10);
                break;
            }

            while (*ptr++) {}
        }

        close(fd);
    }

    free(procs);

    return pid;
}

/* ************************************************************************** */
/* Look to see if name corresponds to a running process. Return PID if it
 * does. */
char ** elm_sys_get_proc(void)
{
    DIR            *dstream = opendir("/proc");
    char          **procs   = calloc(1, sizeof(*procs));
    char           *endptr;
    struct dirent  *entry;
    size_t          size    = 1;
    size_t          length  = 0;

    while ((entry=readdir(dstream))) {
        if (entry->d_type != DT_DIR) {
            continue;
        }

        if (!strtol(entry->d_name, &endptr, 10) || (*endptr != '\0')) {
            continue;
        }

        length        = strlen(entry->d_name)+1;
        procs[size-1] = calloc(length, sizeof(*procs[0]));

        strncpy(procs[size-1], entry->d_name, length);

        procs = realloc(procs, (++size)*sizeof(*procs));
    }

    closedir(dstream);

    procs[size-1] = NULL;

    return procs;
}
