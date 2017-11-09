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
#include <stdarg.h>
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
/* Allocate memory then copy a string to allocated memory */
char * elm_string_copy(char **ptr, char *string)
{
    size_t length = strlen(string)+1;

    if (!elm_calloc(ptr, length, sizeof *string)) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to allocate memory for string", string);
        return NULL;
    }

    strncpy(*ptr, string, length);

    return *ptr;
}

/* ************************************************************************** */
/* Wrapper for calloc() that does pre and post checking for you. This will not
 * allocate memory for a pointer that is not null. If ptr is non-nll, should be
 * reference to another pointer. */
void * elm_calloc(void *ptr, size_t nmemb, size_t size)
{
    /* Call calloc directly when pointer is null */
    if (!ptr) {
        return calloc(nmemb, size);
    }

    char **cast = ptr;
    void  *new;

    /* Do not allocate memory for a non-null pointer */
    if (*cast) {
        return *cast;
    }

    /* Allocate memory */
    if (!(new=calloc(nmemb, size))) {
        return NULL;
    }

    return (*cast=new);
}

/* ************************************************************************** */
/* Wrapper for calloc() that does post checking for you. If ptr is non-null,
 * should be reference to another pointer. */
void * elm_realloc(void *ptr, size_t nmemb, size_t size)
{
    /* Call calloc directly when pointer is null */
    if (!ptr) {
        return elm_calloc(NULL, nmemb, size);
    }

    /* Reallocate memory */
    char **cast = ptr;
    void  *new;

    if (!(new=realloc(*cast, nmemb*size))) {
        free(new);
        return NULL;
    }

    return (*cast=new);
}

/* ************************************************************************** */
/* Wrapper for free(), for completeness since other *alloc functions were
 * wrapped. */
void elm_free(void *ptr)
{
    free(ptr);
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
    char         *fpath;
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
        fpath = elm_sys_path("/proc/%s/cmdline", proc);

        if ((stat(fpath, &info) < 0) \
            || ((info.st_uid != uid) && (uid > 0)) \
            || (access(fpath, R_OK) < 0))
        {
            continue;
        }

        /* Read cmdline file for pid */
        if (((fd=open(fpath, O_RDONLY)) < 0) \
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
        free(procs[index++]);
        procs[index-1] = NULL;

        if (procs[index]) {
            return procs[index];
        }
        else {
            goto maincleanup;
        }
    }
    else {
        index = 1;
    }

    /* Populate process list with current processes */
    DIR            *dhandle = opendir("/proc");
    struct dirent  *entry;
    char           *endptr;

    if (!dhandle) {
        index = 0;
        return NULL;
    }

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
        if (!elm_calloc(&procs, 1, sizeof *procs)) {
            elmprintf(LOGERRNO, "Unable to allocate process array");
            goto cleanup;
        }

        /* Copy string to process list */
        if (!elm_string_copy(&procs[index-1], entry->d_name)) {
            goto cleanup; /* Should be maincleanup, make label more robust */
        }

        /* Increase size of allocated memory region */
        if (!elm_realloc(&procs, index+1, sizeof *procs)) {
            elmprintf(LOGERRNO, "Unable to reallocate process array");
            goto cleanup; /* Should be maincleanup, make label more robust */
        }

        procs[index++] = NULL;
    }

    closedir(dhandle);

    index = 0;

    return *procs;

/* Clear all allocated memory */
maincleanup:
    free(procs[index]);

    procs[index] = NULL;

cleanup:
    free(procs);

    procs = NULL;
    index = 0;

    return NULL;
}

/* ************************************************************************** */
/* Return path given path components */
char * elm_sys_path(const char *format, ...)
{
    static char path[ELM_MAX_PATH_SIZE];
    va_list     ap;

    va_start(ap, format);
    vsnprintf(path, sizeof(path), format, ap);
    va_end(ap);

    return path;
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
