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
/* Set environment variables */
int elm_setenv(char *name, char *value)
{
    elmprintf(LOG, "Setting environment variable: '%s=%s'.", name, value);
    if (setenv(name, value, 1) < 0) {
        elmprintf(LOG, "Unable to set environment variable '%s=%s': %s.", name,
                  value, strerror(errno));
        return 1;
    }

    return 0;
}

/* ************************************************************************** */
/* Return the basename of a path*/
char * basename(const char *string)
{
    /* Copy string contents */
    static  char buf[128];
    size_t  size = strlen(string)+1;
    if (sizeof(buf) < size)
        size = sizeof(buf);
    memcpy(buf, string, size);
    char   *ret   = &*buf;

    /* Find path basename */
    char    sep   = '/';
    size_t  shift = 0;
    size_t  i     = 0;
    while ( ret[i] != '\0' ) {
        if ( ret[i] == sep )
            shift = i+1;
        ++i;
    }
    ret += shift;
    return ret;
}

/* Read process info/enironment variables */
/* char * pinfo/penv() */

#include "elmio.h"

/* ************************************************************************** */
long pgrep(const char *program)
{
    DIR  *dstream = opendir("/proc");
    uid_t uid     = getuid();
    char *ptr;
    char *end;
    char  fpath[64];
    char  buffer[256];
    int   fd;
    int   nbytes;
    pid_t pid;
    struct dirent *entry;
    struct stat    buf;

    /* Loop through process directory */
    while ((entry=readdir(dstream)) != NULL)
    {
        /* Determine directory name (equates to PID) */
        if (entry->d_type != DT_DIR)
            continue;
        pid = strtol(entry->d_name, &ptr, 10);
        if ((pid == 0) || (*ptr != '\0'))
            continue;

        /* Check PID file permissions */
        snprintf(fpath, sizeof(fpath), "/proc/%d/cmdline", pid);
        if (stat(fpath, &buf) != 0)
            continue;
        if ((uid > 0) && (buf.st_uid != uid))
            continue;
        if (access(fpath, R_OK))
            continue;

        /* Read PID file */
        if ((fd=open(fpath, O_RDONLY)) < 0)
            continue;
        if ((nbytes=read(fd, buffer, sizeof buffer)) <= 0)
            continue;

        /* Parse PID file */
        end = buffer + nbytes;
        for (ptr=buffer; ptr < end; ) {
            if (strstr(ptr, program) != NULL)
                return pid;
            while(*ptr++);
        }
        close(fd);
    }
    closedir(dstream);

    return 0;
}

/* ************************************************************************** */
/* Return substring found in line */
char * substring(const char *string, char *line)
{
    size_t  i = 0;
    char    c;
    while ((c=line[i]) != '\0') {
        if (isspace(c)) {
            line[i] = '\0';
            break;
        }
        if (c == string[i])
            ++i;
        else {
            ++line;
            i = 0;
        }
    }
    return (i > 0) ? line : NULL;
}

/* ************************************************************************** */
/* Store command output as a string inside variable */
void get_cmd_output(char *arr, int size, char *cmd) /* Add size to parameters */
{
    FILE *stream = popen(cmd, "r");
    size_t i     = 0;
    size_t j     = 0;
    char line[512]; /* Remove this const value */
    memset(arr, 0, size);

    /* Append process output to return array */
    while ( fgets(line, sizeof(line), stream) != 0 ) {
        i = 0;
        while ( (line[i] != '\0') && (j < size) ) {
            arr[j] = line[i];
            ++i;
            ++j;
        }
    }
    pclose(stream);
    arr[size-1] = 0;
}

/* ************************************************************************** */
/* Execute process in the background */
pid_t exec_proc(char *cmd)
{
    pid_t pid = fork();
    if (pid == 0)
        _exit(system(cmd));
    wait(NULL);
    return pid;
}
