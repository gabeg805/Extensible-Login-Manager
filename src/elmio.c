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
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Private functions */
static char * modetostr(ElmPrint_t mode);
static char * strtimenow(const char *fmt);
static void infoprintf(const char *fmt, va_list ap);
static void warnprintf(const char *fmt, va_list ap);
static void errprintf(const char *fmt, va_list ap);
static void logprintf(const char *fmt, va_list ap);
static void debugprintf(const char *fmt, va_list ap);

/* Private variables */
static int Verbose = 0;

/* ************************************************************************** */
/* Print */
void elmprintf(ElmPrint_t mode, const char *fmt, ...)
{
    /* Format line label */
    static const char *timefmt = "%Y-%m-%d %I:%M:%S %p";
    static char label[128];
    static char linefmt[256];
    snprintf(label, sizeof(label), "[%s] %s:", strtimenow(timefmt),
             modetostr(mode));
    snprintf(linefmt, sizeof(linefmt), "%s %s\n", label, fmt);

    /* Call appropriate logging function */
    va_list arglist;
    va_start(arglist, fmt);
    if (mode == INFO) {
        infoprintf(linefmt, arglist);
    }
    else if (mode == WARNING) {
        warnprintf(linefmt, arglist);
    }
    else if (mode == ERROR) {
        errprintf(linefmt, arglist);
    }
    else if (mode == LOG) {
        /* infoprintf(linefmt, arglist); */
        logprintf(linefmt, arglist);
    }
    else if (mode == DEBUG) {
        debugprintf(linefmt, arglist);
    }
    else if (mode == NONE) {
        infoprintf(linefmt, arglist);
    }
    else
        ;
    va_end(arglist);
}

/* ************************************************************************** */
/* Convert print mode to string */
char * modetostr(ElmPrint_t mode)
{
    static char str[10];
    if (mode == INFO)
        strncpy(str, "INFO", 4);
    else if (mode == WARNING)
        strncpy(str, "WARNING", 7);
    else if (mode == ERROR)
        strncpy(str, "ERROR", 5);
    else if (mode == DEBUG)
        strncpy(str, "DEBUG", 5);
    else if (mode == LOG)
        strncpy(str, "LOG", 3);
    else
        str[0] = '\0';

    uint8_t len = strlen(str);
    if (len >= 10)
        len = 9;
    str[len] = '\0';
    return str;
}

/* ************************************************************************** */
/* Return current time string */
char * strtimenow(const char *fmt)
{
    time_t       now     = time(0);
    struct tm   *tstruct = localtime(&now);
    static char  buf[128];
    memset(&buf, 0, sizeof(buf));
    strftime(buf, sizeof(buf), fmt, tstruct);
    return buf;
}

/* ************************************************************************** */
/* Print informational messages to stdout */
void infoprintf(const char *fmt, va_list ap)
{
    vprintf(fmt, ap);
}

/* ************************************************************************** */
/* Print warning messages to stdout */
void warnprintf(const char *fmt, va_list ap)
{
    vprintf(fmt, ap);
}

/* ************************************************************************** */
/* Print error messages to stderr */
void errprintf(const char *fmt, va_list ap)
{
    vfprintf(stderr, fmt, ap);
}

/* ************************************************************************** */
/* Print messages for logging to log */
void logprintf(const char *fmt, va_list ap)
{
    FILE *stream = fopen(ELM_LOG, "a+");
    vfprintf(stream, fmt, ap);
    fclose(stream);
}

/* ************************************************************************** */
/* Print debug messages to stdout and to the log */
void debugprintf(const char *fmt, va_list ap)
{
    infoprintf(fmt, ap);
    logprintf(fmt, ap);
}

/* ************************************************************************** */
/* Set verbose flag */
void elm_set_verbose(int flag)
{
    Verbose = flag;
}
