/* *****************************************************************************
 * 
 * Name:    utility.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Common functions used by the Elysia Login Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "utility.h"
#include "elytype.h"
#include "elyglobal.h"
#include "benchmark.h"
#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <gtk/gtk.h>



/* *************************
 * ***** PROGRAM USAGE *****
 * *************************
 */

void usage()
{
    printf("Usage: %s [option]\n", PROG);
    printf("\n");
    printf("Options:\n");
    printf("\t-h, --help       Print program usage.\n");
    printf("\t-v, --verbose    Print program information verbosely.\n");
    printf("\t-p, --preview    Run the login manager in Preview Mode, does not paint a new X window.\n");
    printf("\t-t, --time       Run benchmark time tests.\n");
}



/* ******************************* */
/* ***** PARSE INPUT OPTIONS ***** */
/* ******************************* */

/* Parse the command line arguments */
void parse_argv(int argc, char **argv)
{
    const struct option long_options[] = {
        {"help",    optional_argument, 0, 'h'},
        {"verbose", optional_argument, 0, 'v'},
        {"preview", optional_argument, 0, 'p'},
        {"time",    optional_argument, 0, 't'},
        {0, 0, 0, 0}
    };

    int opt;
    while ( (opt=getopt_long(argc, argv, "hvpt", long_options, 0)) != -1 ) {
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
        case 't':
            BENCHTIME = true;
            break;
        default:
            break;
        }
    }
}



/* ********************************
 * ***** STRING MANIPULATIONS *****
 * ********************************
 */

/* Return the basename of a path*/
char * basename(char *str)
{
    size_t shift = 0;
    size_t i     = 0;
    char sep     = '/';
    while ( str[i] != 0 ) {
        if ( str[i] == sep )
            shift = i+1;
        ++i;
    }
    str += shift;

    return str;
}

/* Return the substring between the given separator */
void get_substring(char *copy, char *str, char sep, int num)
{
    size_t i     = 0;
    size_t j     = 0;
    size_t count = 1; /* reduce this so that it's by index */

    while ( str[i] != '\0' ) {
        if ( str[i] == sep )
            ++count;
        else if ( count == num ) {
            copy[j] = str[i];
            ++j;
        }

        if ( count > num ) 
            break;

        ++i;
    }
    copy[j] = 0;
}



/* *************************
 * ***** WRITE TO FILE *****
 * *************************
 */

/* Write to the Elysia log file */
void file_log(const char *fmt, ...)
{
    char *file = ELYSIA_LOG;
    char *opt = "a+";

    FILE *stream = fopen(file, opt);
    va_list args;

    va_start(args, fmt);
    vfprintf(stream, fmt, args);
    va_end(args);

    fclose(stream);
}



/* Overwrite configuration file line that matches the given key */
void file_line_overwrite(char *file, char *key, char *val)
{
    /* Prepare new strings for the file overwrite procedure */
    char *ext  = ".bak";
    size_t n   = strlen(file) + strlen(ext) + 1;
    size_t len = strlen(key)  + strlen(val) + 3;
    char newfile[n];
    char replacement[len];
    snprintf(newfile,     n,   "%s%s",   file, ext);
    snprintf(replacement, len, "%s: %s", key,  val);

    /* Edit the line matching the key */
    FILE *stream    = fopen(file, "r+");
    FILE *newstream = fopen(newfile, "w");
    bool write      = false;
    char line[MAX_STR_LEN];
    char copy[MAX_STR_LEN];
    while ( fgets(line, sizeof(line), stream) != 0 ) {
        get_substring(copy, line, ':', 1);

        if ( (strcmp(key, copy) == 0) ) {
            fprintf(newstream, "%s\n", replacement);
            write = true;
        } 
        else 
            fprintf(newstream, "%s", line);
    }
    
    /* Write in default entry if key was not found */
    if ( !write )
        fprintf(stream, "%s: Not Found\n", key);

    fclose(stream);
    fclose(newstream);
    remove(file);
    rename(newfile, file);
}



/* *************************************** */
/* ***** CHECK IF PROGRAM IS RUNNING ***** */
/* *************************************** */

/* Check if a program is running */
bool is_running(char *prog)
{
    DIR  *dstream = opendir("/proc");
    FILE *fstream;
    struct dirent *entry;
    char *endptr;
    char buf[512];
    long pid;

    /* Loop through process directory */
    while ( (entry=readdir(dstream)) != 0 ) {
        /* Determine process command line file */
        pid = strtol(entry->d_name, &endptr, 10);
        if ( *endptr != '\0' ) 
            continue;
        snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", pid);

        /* Read the command line file */
        fstream = fopen(buf, "r");
        if ( fgets(buf, sizeof(buf), fstream) != 0 ) {
            size_t i    = 0;
            size_t size = strlen(buf) + 1;
            char copy[size];
            memset(copy, 0, size);

            /* Compare first word in command line call with program name */
            while ( (buf[i] != '\0') && (buf[i] != ' ') ) {
                copy[i] = buf[i];
                ++i;
            }
            if ( strcmp(copy, prog) == 0 ) 
                return true;
        }
        fclose(fstream);
    }
    closedir(dstream);

    return false;
}



/* ************************************ */
/* ***** GET LINUX COMMAND OUTPUT *****  */
/* ************************************ */

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
