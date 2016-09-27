/* *****************************************************************************
 * 
 * Name:    utility.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Common functions used by the Elysia Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "utility.h"
#include "elyglobal.h"
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <gtk/gtk.h>

#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include "elm.h"

/* ************************************************************************** */
/* Print program usage */
void usage()
{
    printf("Usage: %s [option]\n", PROGRAM);
    printf("\n");
    printf("Options:\n");
    printf("\t-h, --help       Print program usage.\n");
    printf("\t-v, --verbose    Print program information verbosely.\n");
    printf("\t-p, --preview    Run the login manager in Preview Mode, does not paint a new X window.\n");
}

/* ************************************************************************** */
/* Parse the command line arguments */
void parse_argv(int argc, char **argv)
{
    const struct option long_options[] = {
        {"help",    optional_argument, 0, 'h'},
        {"verbose", optional_argument, 0, 'v'},
        {"preview", optional_argument, 0, 'p'},
        {0, 0, 0, 0}
    };

    int opt;
    while ( (opt=getopt_long(argc, argv, "hvp", long_options, 0)) != -1 ) {
        switch (opt) {
        case 'h':
            usage(argv[0]);
            exit(0);
            break;
        case 'v':
            VERBOSE = true;
            break;
        case 'p':
            PREVIEW = true;
            break;
        default:
            break;
        }
    }
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

/* ************************************************************************** */
long pgrep(const char *program)
{
    DIR  *dstream = opendir("/proc");
    uid_t uid     = getuid();
    FILE *fstream;
    char *endptr;
    char  fpath[64];
    char  fcontents[128];
    char  substr[128];
    pid_t pid;
    struct dirent *entry;
    struct stat    buf;

    /* Loop through process directory */
    while ((entry=readdir(dstream)) != NULL) {
        /* Determine directory name (equates to PID) */
        if (entry->d_type != DT_DIR)
            continue;
        pid = strtol(entry->d_name, &endptr, 10);
        if ((pid == 0) || (*endptr != '\0'))
            continue;

        /* Check PID file permissions */
        snprintf(fpath, sizeof(fpath), "/proc/%d/cmdline", pid);
        if (stat(fpath, &buf) != 0)
            continue;
        if ((uid > 0) && (buf.st_uid != uid))
            continue;
        if (access(fpath, R_OK))
            continue;
        fstream = fopen(fpath, "r");
        if (fstream == NULL)
            continue;

        /* Read PID file */
        if (fgets(fcontents, sizeof(fcontents), fstream) != NULL) {
            memcpy(substr, fcontents, sizeof(fcontents));
            if (substring(program, substr) != NULL) {
                return pid;
            }
        }
        fclose(fstream);
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
