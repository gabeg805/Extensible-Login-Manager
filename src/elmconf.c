/* *****************************************************************************
 * 
 * Name:    elmconf.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Parse config files.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elmconf.h"
#include "elmdef.h"
#include "elmio.h"
#include "elmsys.h"
#include <glib.h>
#include <stdbool.h>

#include <stdio.h>

/* Private functions */
/* To-do: static char -> malloc */
int elm_conf_key_file(GKeyFile **keyfile, const char *configfile);

/* Private variables */
const char *ConfigFile = "/etc/X11/elm/etc/elm.conf";

/* ************************************************************************** */
/* Read the configuration file and return the key value as a string */
char * elm_conf_read(const char *group, const char *key)
{
    GError      *err = NULL;
    GKeyFile    *keyfile;
    char        *value;

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return NULL;
    }

    value = g_key_file_get_value(keyfile, group, key, &err);

    if (elm_is_key_err(&err)) {
        return NULL;
    }

    return elm_sys_strcpy(NULL, value);
}

/* ************************************************************************** */
/* Read the configuration file and return the key value as an unescaped UTF-8
 * string */
char * elm_conf_read_str(const char *group, const char *key)
{
    GError      *err = NULL;
    GKeyFile    *keyfile;
    char        *value;
    static char  read[ELM_MAX_CONF_SIZE];

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return NULL;
    }

    value = g_key_file_get_string(keyfile, group, key, &err);

    if (elm_is_key_err(&err)) {
        return NULL;
    }

    strncpy(read, value, sizeof(read)-1);

    return read;
}

/* ************************************************************************** */
/* Read the configuration file and return the key value as an int */
int elm_conf_read_int(const char *group, const char *key)
{
    GError   *err = NULL;
    GKeyFile *keyfile;
    gint      read;

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return -1;
    }

    read = g_key_file_get_integer(keyfile, group, key, &err);

    if (elm_is_key_err(&err)) {
        return -2;
    }

    return read;
}

/* ************************************************************************** */
/* Read the configuration file and return the key value as a bool */
bool elm_conf_read_bool(const char *group, const char *key)
{
    GError   *err = NULL;
    GKeyFile *keyfile;
    gboolean  read;

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return -1;
    }

    read = g_key_file_get_boolean(keyfile, group, key, &err);

    if (elm_is_key_err(&err)) {
        return -2;
    }

    return read;
}

/* ************************************************************************** */
/* Return key populated by config file */
int elm_conf_key_file(GKeyFile **keyfile, const char *configfile)
{
    GKeyFileFlags  flags = G_KEY_FILE_NONE;
    GError        *err   = NULL;

    *keyfile = g_key_file_new();

    if (!g_key_file_load_from_file(*keyfile, configfile, flags, &err)) {
        elm_is_key_err(&err);
        *keyfile = NULL;
        return -1;
    }

    return 0;
}

/* ************************************************************************** */
/* Return all groups in the config file */
char ** elm_conf_get_groups(void)
{
    GKeyFile  *keyfile;
    char     **groups;

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return NULL;
    }

    groups = g_key_file_get_groups(keyfile, NULL);

    return groups;
}

/* ************************************************************************** */
/* Return all keys for a given group in the config file */
char ** elm_conf_get_keys(const char *group)
{
    GError    *err = NULL;
    GKeyFile  *keyfile;
    char     **keys;

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return NULL;
    }

    keys = g_key_file_get_keys(keyfile, group, NULL, &err);

    if (elm_is_key_err(&err)) {
        return NULL;
    }

    return keys;
}

/* ************************************************************************** */
/* Check if an error occurred */
int elm_is_key_err(GError **err)
{
    if (*err) {
        elmprintf(LOGERR, (*err)->message);
        g_error_free(*err);
        return 1;
    }

    return 0;
}
