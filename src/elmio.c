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
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

/* Private functions */
static void   elm_io_info(char *preamble, va_list ap);
static void   elm_io_warn(char *preamble, va_list ap);
static void   elm_io_err(char *preamble, va_list ap);
static void   elm_io_log(char *preamble, va_list ap);
static char * elm_io_get_preamble(ElmPrint mode, const char *vafmt);
static char * elm_io_get_time(void);
static int    elm_io_is_mode_info(ElmPrint mode);
static int    elm_io_is_mode_warn(ElmPrint mode);
static int    elm_io_is_mode_err(ElmPrint mode);
static int    elm_io_is_mode_log(ElmPrint mode);
static int    elm_io_is_mode_errno(ElmPrint mode);
static char * elm_io_mode_to_string(ElmPrint mode);

/* Private variables */
static int      Verbose      = 0;
static uint32_t InfoMask     = 0x3;
static uint32_t WarnMask     = 0xc;
static uint32_t ErrorMask    = 0x30;
static uint32_t LogThreshold = 0x40;
static uint32_t LogIMask     = 0xc0;
static uint32_t LogWMask     = 0x300;
static uint32_t LogEMask     = 0xc00;

/* ************************************************************************** */
/* Print wrapper */
void elmprintf(ElmPrint mode, const char *vafmt, ...)
{
    /* Call appropriate printing function */
    char    *preamble = elm_io_get_preamble(mode, vafmt);
    va_list  ap;
    va_list  aq;

    va_start(ap, vafmt);
    va_copy(aq, ap);

    /* Print to stdout/stderr */
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

    /* Print to log */
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
/* Return the preamble of the print statement */
char * elm_io_get_preamble(ElmPrint mode, const char *vafmt)
{
    static char preamble[ELM_MAX_MSG_SIZE];
    int         available = sizeof(preamble);
    int         length;

    /* Default preamble (excluding trailing newline) */
    snprintf(preamble, available, "[%s] %s %s",
             elm_io_get_time(), elm_io_mode_to_string(mode), vafmt);

    length     = strlen(preamble)+1;
    available -= (length+1);

    /* Add errno message to string */
    if (elm_io_is_mode_errno(mode)) {
        char   *errmsg = strerror(errno);
        size_t  errlen = strlen(errmsg)+4;

        if (available >= errlen) {
            strncat(preamble, ": ", 2);
            strncat(preamble, errmsg, errlen-1);
            strncat(preamble, ".", 1);

            available -= errlen;
        }
    }

    /* Add newline */
    if (available > 0) {
        strncat(preamble, "\n", 1);
    }

    return preamble;
}

/* ************************************************************************** */
/* Return current time string */
char * elm_io_get_time(void)
{
    static const char *fmt     = "%Y-%m-%d %I:%M:%S %p";
    time_t             now     = time(0);
    struct tm         *tstruct = localtime(&now);
    static char        buf[128];

    memset(&buf, 0, sizeof(buf));
    strftime(buf, sizeof(buf), fmt, tstruct);

    return buf;
}

/* ************************************************************************** */
/* Check if INFO mode */
int elm_io_is_mode_info(ElmPrint mode)
{
    return ((mode & InfoMask) || (mode & LogIMask));
}

/* ************************************************************************** */
/* Check if WARNING mode */
int elm_io_is_mode_warn(ElmPrint mode)
{
    return ((mode & WarnMask) || (mode & LogWMask));
}

/* ************************************************************************** */
/* Check if ERROR mode */
int elm_io_is_mode_err(ElmPrint mode)
{
    return ((mode & ErrorMask) || (mode & LogEMask));
}

/* ************************************************************************** */
/* Check if LOG mode */
int elm_io_is_mode_log(ElmPrint mode)
{
    return (mode > LogThreshold);
}

/* ************************************************************************** */
/* Check if errno mode (any variant that errno) */
int elm_io_is_mode_errno(ElmPrint mode)
{
    return ((mode == INFNO)       \
            || (mode == WARNO)    \
            || (mode == ERRNO)    \
            || (mode == LOGINFNO)  \
            || (mode == LOGWARNO) \
            || (mode == LOGERRNO));
}

/* ************************************************************************** */
/* Convert print mode to string */
char * elm_io_mode_to_string(ElmPrint mode)
{
    static char str[10];

    memset(str, 0, sizeof(str));

    if (elm_io_is_mode_info(mode)) {
        strncpy(str, "INFO ", 6);
    }
    else if (elm_io_is_mode_warn(mode)) {
        strncpy(str, "WARN ", 6);
    }
    else if (elm_io_is_mode_err(mode)) {
        strncpy(str, "ERROR", 6);
    }
    else {
        return "";
    }

    size_t len = strlen(str);
    str[(len >= 10) ? 9 : len] = '\0';

    return str;
}
