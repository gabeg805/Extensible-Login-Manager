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
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

/* Is this needed */
#include <ctype.h>

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
        elmprintf(LOGERRNO, "%s '%s=%s'",
                  "Unable to set environment variable", name, value);
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/* Search for a process that matches the input name and return its PID */
pid_t elm_sys_pgrep(const char *program)
{
    uid_t         uid   = getuid();
    pid_t         pid   = 0;
    char         *proc;
    char         *ptr;
    char         *end;
    char          fpath[ELM_MAX_PATH_SIZE];
    char          buffer[ELM_MAX_MSG_SIZE];
    struct stat   info;
    int           nbytes;
    int           fd;

    /* Search for a process that matches the input name */
    while ((proc=elm_sys_get_proc()))
    {
        /* Focus on freeing memory once pid is set */
        if (pid > 0) {
            continue;
        }

        /* Make sure user spawned process */
        snprintf(fpath, sizeof(fpath), "/proc/%s/cmdline", proc);

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
                pid = strtol(proc, 0, 10);
                break;
            }

            while (*ptr++) {}
        }

        close(fd);
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
char * elm_sys_get_proc(void)
{
    static char   **procs = NULL;
    static size_t   index = 0;

    /* Return next process in list */
    if (procs) {
        /* Clear previous memory */
        free(procs[index++]);
        procs[index-1] = NULL;

        if (procs[index]) {
            return procs[index];
        }
        else {
            /* Clear all allocated memory */
            free(procs[index]);
            free(procs);
            procs = NULL;
            index = 0;

            return NULL;
        }
    }

    /* Populate process list with current processes */
    DIR            *dstream = opendir("/proc");
    size_t          size    = 0;
    size_t          length  = 0;
    struct dirent  *entry;
    char           *endptr;

    while ((entry=readdir(dstream)))
    {
        /* Must be a directory */
        if (entry->d_type != DT_DIR) {
            continue;
        }

        /* Must have a name that can be converted to a number */
        if (!strtol(entry->d_name, &endptr, 10) || (*endptr != '\0')) {
            continue;
        }

        /* Allocate memory for process list if not already allocated */
        if (!procs) {
            size  = 1;
            procs = calloc(size, sizeof(*procs));
            /* To-do: Add check for null */
        }

        /* Copy string to process list */
        length        = strlen(entry->d_name)+1;
        procs[size-1] = calloc(length, sizeof(*procs[0]));

        strncpy(procs[size-1], entry->d_name, length);

        procs = realloc(procs, (++size)*sizeof(*procs));
    }

    /* Cleanup */
    closedir(dstream);

    if (size > 0) {
        procs[size-1] = NULL;
    }

    return *procs;
}

/* ************************************************************************** */
/* Return first line from file */
char * elm_sys_read_line(char *file)
{
    if (access(file, R_OK)) {
        return NULL;
    }

    FILE   *fstream                 = fopen(file, "r");
    char    line[ELM_MAX_PATH_SIZE] = {0};
    char   *pos;
    size_t  length;

    /* Read file */
    while (fgets(line, sizeof(line), fstream)) {
        if ((pos=strchr(line, '\n'))) {
            *pos = 0;
        }

        if ((length=strlen(line)+1) <= 1) {
            continue;
        }

        break;
    }

    /* Line was empty */
    if (length <= 1) {
        return NULL;
    }

    /* Return line */
    char *read = calloc(length, sizeof(*read));

    strncpy(read, line, length);

    return read;
}
