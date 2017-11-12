/* -----------------------------------------------------------------------------
 * 
 * Name:    elmstr.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Useful string functions.
 *              
 * Notes: None.
 * 
 * -----------------------------------------------------------------------------
 */

/* Includes */
#include "elmstr.h"
#include "elmdef.h"
#include "elmio.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* -------------------------------------------------------------------------- */
/* Copy string */
char * elm_str_copy(char *string)
{
    char *dup = NULL;

    if (!string) {
        return NULL;
    }

    if (!(dup=strdup(string))) {
        elmprintf(LOGERRNO, "%s '%s'",
                  "Unable to allocate memory for string", string);
        return NULL;
    }

    return dup;
}

/* -------------------------------------------------------------------------- */
/* Copy string from variable arg list */
char * elm_str_vcopy(size_t size, const char *format, ...)
{
    char     buffer[size];
    va_list  ap;

    va_start(ap, format);
    vsnprintf(buffer, size, format, ap);
    va_end(ap);

    return elm_str_copy(buffer); 
}

/* -------------------------------------------------------------------------- */
/* Return path from variable arg list */
char * elm_str_path(const char *format, ...)
{
    char     buffer[ELM_MAX_PATH_SIZE];
    va_list  ap;

    va_start(ap, format);
    vsnprintf(buffer, sizeof buffer, format, ap);
    va_end(ap);

    return elm_str_copy(buffer);
}

/* -------------------------------------------------------------------------- */
/* Return first non-empty line from file */
char * elm_str_readline(char *file)
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
    char *strip;

    while (fgets(line, sizeof(line), fhandle)) {
        strip = elm_str_strip(line);

        /* Non-empty line found */
        if (strlen(strip) > 1) {
            if (!(read=elm_str_copy(strip))) {
                elmprintf(LOGERR, "Unable to read line '%s'", strip);
            }

            break;
        }
    }

    fclose(fhandle);

    return read;
}

/* -------------------------------------------------------------------------- */
/* Return the first line that matches the specified substring */
char * elm_str_findline(char *file, char *substring)
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
    char *strip;

    while (fgets(line, sizeof(line), fhandle)) {
        strip = elm_str_strip(line);

        /* Match found */
        if (strstr(strip, substring)) {
            if (!(read=elm_str_copy(strip))) {
                elmprintf(LOGERR, "Unable to read line '%s'", strip);
            }

            break;
        }
    }

    fclose(fhandle);

    return read;
}

/* -------------------------------------------------------------------------- */
/* Strip trailing newline */
char * elm_str_strip(char *string)
{
    size_t  size = strlen(string);
    char   *end;

    if (!size) {
        return string;
    }

    for (end=string+size-1; (end >= string) && isspace(*end); end--) {}

    *(end+1) = '\0';

    for ( ; *string && isspace(*string); string++) {}

    return string;
}
