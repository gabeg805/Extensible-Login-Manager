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
#include "elmalloc.h"
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
char * elm_sys_strcpy(char **ptr, char *string)
{
    size_t   length = strlen(string)+1;
    char    *copy   = NULL;
    char   **ref;

    if (!ptr) {
        ref = &copy;
    }
    else {
        ref = ptr;
    }

    if (!elm_calloc(ref, length, sizeof *string)) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to allocate memory for string", string);
        return NULL;
    }

    strncpy(*ref, string, length);

    return *ref;
}

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
        fpath = elm_sys_path("/proc/%s/cmdline", proc[i]);

        if ((stat(fpath, &info) < 0) \
            || ((info.st_uid != uid) && (uid > 0)) \
            || (access(fpath, R_OK) < 0))
        {
            elm_free(&fpath);
            elm_free(&proc[i]);
            continue;
        }

        /* Read cmdline file for pid */
        if (((fd=open(fpath, O_RDONLY)) < 0) \
            || ((nbytes=read(fd, buffer, sizeof(buffer))) <= 0))
        {
            elm_free(&fpath);
            elm_free(&proc[i]);
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
        elm_free(&fpath);
        elm_free(&proc[i]);
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
            if (!elm_calloc(&procs, 1, sizeof *procs)) {
                elmprintf(LOGERRNO, "Unable to allocate process array");
                goto cleanup;
            }
        }

        /* Copy string to process list */
        if (!elm_sys_strcpy(&procs[index-1], entry->d_name)) {
            goto cleanup;
        }

        /* Increase size of allocated memory region */
        if (!elm_realloc(&procs, index+1, sizeof *procs)) {
            elmprintf(LOGERRNO, "Unable to reallocate process array");
            goto cleanup;
        }

        procs[index++] = NULL;
    }

    closedir(dhandle);

    return procs;

cleanup:
    for (i=0; i < index; i++) {
        elm_free(&procs[i]);
    }

    elm_free(&procs);

    return NULL;
}

/* ************************************************************************** */
/* Return path given path components */
char * elm_sys_path(const char *format, ...)
{
    char    *path = NULL;
    char     buffer[ELM_MAX_PATH_SIZE];
    va_list  ap;

    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    return elm_sys_strcpy(&path, buffer);
}

/* ************************************************************************** */
/* Return first line from file */
char * elm_sys_read_line(char *file)
{
    FILE *fhandle = NULL;
    char *read    = NULL;

    /* Make sure file can be read */
    if (access(file, R_OK)) {
        return NULL;
    }

    if (!(fhandle=fopen(file, "r"))) {
        return NULL;
    }

    /* Read file */
    char  line[ELM_MAX_PATH_SIZE] = {0};
    char *pos;

    while (fgets(line, sizeof(line), fhandle)) {
        if ((pos=strchr(line, '\n'))) {
            *pos = 0;
        }

        /* Suitable line found */
        if (strlen(line) > 1) {
            if (!elm_sys_strcpy(&read, line)) {
                elmprintf(LOGERR, "Unable to read line '%s'", line);
            }

            break;
        }
    }

    fclose(fhandle);

    return read;
}

/* ************************************************************************** */
/* Return the first line that matches the specified substring */
char * elm_sys_find_line(char *file, char *substring)
{
    FILE *fhandle = NULL;
    char *read    = NULL;

    /* Make sure file can be read */
    if (access(file, R_OK)) {
        return NULL;
    }

    if (!(fhandle=fopen(file, "r"))) {
        return NULL;
    }

    /* Read file */
    char  line[ELM_MAX_PATH_SIZE] = {0};
    char *pos;

    while (fgets(line, sizeof(line), fhandle)) {
        if ((pos=strchr(line, '\n'))) {
            *pos = 0;
        }

        /* Match found */
        if (strstr(line, substring)) {
            if (!elm_sys_strcpy(&read, line)) {
                elmprintf(LOGERR, "Unable to read line '%s'", line);
            }

            break;
        }
    }

    fclose(fhandle);

    return read;
}
