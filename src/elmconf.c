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
#include <glib.h>
#include <stdbool.h>

#include <stdio.h>

/* Private functions */
int     elm_conf_key_file(GKeyFile **keyfile, const char *configfile);
char ** elm_conf_get_groups(void);
char ** elm_conf_get_keys(char *group);
int     elm_conf_is_valid_request(GKeyFile *keyfile, char *group, char *key);
int     elm_conf_is_key_err(GError *err);

/* Private variables */
const char *ConfigFile = "/etc/X11/elm/conf/elm.conf";

/* ************************************************************************** */
/* Read the configuration file and return the key value as a string */
char * elm_conf_read(char *group, char *key)
{
    GError      *err = NULL;
    GKeyFile    *keyfile;
    char        *value;
    static char  read[ELM_MAX_CONF_SIZE];

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return NULL;
    }

    value = g_key_file_get_value(keyfile, group, key, &err);

    if (elm_conf_is_key_err(err)) {
        return NULL;
    }

    strncpy(read, value, sizeof(read)-1);

    return read;
}

/* ************************************************************************** */
/* Read the configuration file and return the key value as an unescaped UTF-8
 * string */
char * elm_conf_read_str(char *group, char *key)
{
    GError      *err = NULL;
    GKeyFile    *keyfile;
    char        *value;
    static char  read[ELM_MAX_CONF_SIZE];

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return NULL;
    }

    value = g_key_file_get_string(keyfile, group, key, &err);

    if (elm_conf_is_key_err(err)) {
        return NULL;
    }

    strncpy(read, value, sizeof(read)-1);

    return read;
}

/* ************************************************************************** */
/* Read the configuration file and return the key value as an int */
int elm_conf_read_int(char *group, char *key)
{
    GError   *err = NULL;
    GKeyFile *keyfile;
    gint      read;

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return -1;
    }

    read = g_key_file_get_integer(keyfile, group, key, &err);

    if (elm_conf_is_key_err(err)) {
        return -2;
    }

    return read;
}

/* ************************************************************************** */
/* Read the configuration file and return the key value as a bool */
bool elm_conf_read_bool(char *group, char *key)
{
    GError   *err = NULL;
    GKeyFile *keyfile;
    gboolean  read;

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return -1;
    }

    read = g_key_file_get_boolean(keyfile, group, key, &err);

    if (elm_conf_is_key_err(err)) {
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
        printf("Unable to do stuff: %s (Ret:%d | Domain:%d).\n", err->message,
               err->code, err->domain);
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
char ** elm_conf_get_keys(char *group)
{
    GError    *err = NULL;
    GKeyFile  *keyfile;
    char     **keys;

    if (elm_conf_key_file(&keyfile, ConfigFile) < 0) {
        return NULL;
    }

    keys = g_key_file_get_keys(keyfile, group, NULL, &err);

    if (elm_conf_is_key_err(err)) {
        return NULL;
    }

    return keys;
}

/* ************************************************************************** */
/* Check key file for requested group and key */
int elm_conf_is_valid_request(GKeyFile *keyfile, char *group, char *key)
{
    GError *err = NULL;

    if (!g_key_file_has_group(keyfile, group)) {
        return 0;
    }

    if (!g_key_file_has_key(keyfile, group, key, &err)) {
        return 0;
    }

    if (err) {
        return 0;
    }

    return 1;
}

/* ************************************************************************** */
/* Check if an error occurred */
int elm_conf_is_key_err(GError *err)
{
    if (err) {
        printf("Unable to do stuffers: %s (Ret:%d | Domain:%d).\n", err->message,
               err->code, err->domain);
        return 1;
    }

    return 0;
}
