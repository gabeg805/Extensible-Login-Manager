/* *****************************************************************************
 * 
 * Name:    elmio.c
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

/* Includes */
#include "elmio.h"
#include "elmdef.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Private functions */
static void   elm_io_info(char *preamble, va_list ap);
static void   elm_io_warn(char *preamble, va_list ap);
static void   elm_io_err(char *preamble, va_list ap);
static void   elm_io_log(char *preamble, va_list ap);
static int    elm_io_is_mode_info(ElmPrint mode);
static int    elm_io_is_mode_warn(ElmPrint mode);
static int    elm_io_is_mode_err(ElmPrint mode);
static int    elm_io_is_mode_log(ElmPrint mode);
static char * elm_io_mode_to_string(ElmPrint mode);
static char * elm_io_time_now(void);

/* Private variables */
static int Verbose = 0;

/* ************************************************************************** */
/* Print wrapper */
void elmprintf(ElmPrint mode, const char *vafmt, ...)
{
    /* Format line label */
    static char preamble[256];

    snprintf(preamble, sizeof(preamble), "[%s] %s %s\n",
             elm_io_time_now(), elm_io_mode_to_string(mode), vafmt);

    /* Call appropriate printing function */
    va_list ap;
    va_list aq;
    va_start(ap, vafmt);
    va_copy(aq, ap);

    if (elm_io_is_mode_info(mode)) {
        elm_io_info(preamble, ap);
    }
    else if (elm_io_is_mode_warn(mode)) {
        elm_io_warn(preamble, ap);
    }
    else if (elm_io_is_mode_err(mode)) {
        elm_io_err(preamble, ap);
    }
    else {
    }

    if (elm_io_is_mode_log(mode)) {
        elm_io_log(preamble, aq);
    }

    va_end(aq);
    va_end(ap);
}

/* ************************************************************************** */
/* Print informational messages to stdout */
void elm_io_info(char *preamble, va_list ap)
{
    vfprintf(stdout, preamble, ap);
}

/* ************************************************************************** */
/* Print warning messages to stdout */
void elm_io_warn(char *preamble, va_list ap)
{
    vfprintf(stdout, preamble, ap);
}

/* ************************************************************************** */
/* Print error messages to stderr */
void elm_io_err(char *preamble, va_list ap)
{
    vfprintf(stderr, preamble, ap);
}

/* ************************************************************************** */
/* Print messages for logging to log */
void elm_io_log(char *preamble, va_list ap)
{
    FILE *stream = fopen(ELM_LOG, "a+");
    vfprintf(stream, preamble, ap);
    fclose(stream);
}

/* ************************************************************************** */
/* Set verbose flag */
void elm_io_set_verbose(int flag)
{
    Verbose = flag;
}

/* ************************************************************************** */
/* Check if INFO mode */
int elm_io_is_mode_info(ElmPrint mode)
{
    return ((mode == INFO) || (mode == LOGINFO)) ? 1 : 0;
}

/* ************************************************************************** */
/* Check if WARNING mode */
int elm_io_is_mode_warn(ElmPrint mode)
{
    return ((mode == WARNING) || (mode == LOGWARN)) ? 1 : 0;
}

/* ************************************************************************** */
/* Check if ERROR mode */
int elm_io_is_mode_err(ElmPrint mode)
{
    return ((mode == ERROR) || (mode == LOGERR)) ? 1 : 0;
}

/* ************************************************************************** */
/* Check if LOG mode */
int elm_io_is_mode_log(ElmPrint mode)
{
    return ((mode == LOG) \
            || (mode == LOGINFO) \
            || (mode == LOGWARN) \
            || (mode == LOGERR)) \
        ? 1 : 0;
}

/* ************************************************************************** */
/* Convert print mode to string */
char * elm_io_mode_to_string(ElmPrint mode)
{
    static char str[10];

    if (elm_io_is_mode_log(mode)) {
        strncpy(str, "LOG", 3);
    }

    if (elm_io_is_mode_info(mode)) {
        strncpy(str, "INFO", 4);
    }
    else if (elm_io_is_mode_warn(mode)) {
        strncpy(str, "WARNING", 7);
    }
    else if (elm_io_is_mode_err(mode)) {
        strncpy(str, "ERROR", 5);
    }
    else {
        return "";
    }

    size_t len = strlen(str);
    str[(len >= 10) ? 9 : len] = '\0';

    return str;
}

/* ************************************************************************** */
/* Return current time string */
char * elm_io_time_now(void)
{
    static const char *fmt     = "%Y-%m-%d %I:%M:%S %p";
    time_t             now     = time(0);
    struct tm         *tstruct = localtime(&now);
    static char        buf[128];

    memset(&buf, 0, sizeof(buf));
    strftime(buf, sizeof(buf), fmt, tstruct);

    return buf;
}
