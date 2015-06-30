/* *****************************************************************************
 * 
 * Name:    elyconfig.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Syntax: None.
 * 
 * Description: Utilities for reading and parsing the configuration files.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elyconfig.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>



/* ************************ */
/* ***** CONFIG EDITS ***** */
/* ************************ */

/* Overwrite configuration file line that matches the given key */
void overwrite_config_line(char *file, char *key, char *val)
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
    while ( fgets(line, sizeof(line), stream) != NULL ) {
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



/* Execute a command found in the config file */
void exec_config_cmd(char *file, int n)
{
    char key[6];
    snprintf(key, sizeof(key), "cmd%d", n);
    char *cmd = read_config_char(file, key, MAX_CMD_LEN);
    pid_t pid = fork();
    if ( pid == 0 )
        exit(system(cmd));
}



/* Strip a line in a configuration file of the key, separator, and leading 
 * empty space, as well as the trailing newline
 */
char * strip_config_line(char *line, char sep)
{
    /* Remove leading chars */
    bool is_separated = false;
    while ( *line != 0 ) {
        /* Ignore key and separator */
        if ( !is_separated ) {
            if ( *line == sep )
                is_separated = true;
            ++line;
            continue;
        }

        /* Ignore spaces */
        if ( isspace(*line) )
            ++line;
        else
            break;
    }

    /* Remove trailing newline */
    size_t loc = strlen(line)-1;
    char nl    = '\n';
    size_t i   = 0;
    while ( line[i] != 0 ) {
        if ( (i == loc) && (line[i] == nl) )
            break;
        ++i;
    }
    line[i] = 0;

    return line;
}



/* **************************** */
/* ***** READ CONFIG FILE ***** */
/* **************************** */

/* Read the config file and output a string */
char * read_config_char(char *file, char *key, int n)
{
    FILE *stream    = fopen(file, "r");
    char *ret       = NULL;
    char sep        = ':';
    size_t num      = 1;
    char line[n];
    char substr[n];

    /* Loop through line by line the contents of the config file */
    while ( fgets(line, sizeof(line), stream) != NULL ) {
        /* Strip config line's leading components */
        get_substring(substr, line, sep, num);
        if ( strcmp(substr, key) != 0 )
            continue;
        char *stripped = strip_config_line(line, sep);

        /* Copy data from stripped string to return variable */
        size_t len  = strlen(stripped);
        size_t size = len + 1;
        ret = malloc(size);
        assert(ret);
        strcpy(ret, stripped);
        break;
    }
    fclose(stream);

    return ret;
}



/* Read config file and output an int */
int read_config_int(char *file, char *key)
{
    char *val = read_config_char(file, key, MAX_NUM_LEN);
    if ( val == NULL )
        return -1;
    else
        return atoi(val);
}



/* Read a command from the config file and replace the '@' instances with
 * the specified replacements
 */
void read_config_cmd_rep(char *arr, char *file, char *rep1, char *rep2, char *rep3)
{
    size_t num = 0;
    size_t i   = 0;
    size_t j   = 0;
    memset(arr, 0, MAX_CMD_LEN);
    char *cmd = read_config_char(file, "cmd", MAX_CMD_LEN);

    /* Copy command string and replace instances of '@' with the given parameters */
    while ( cmd[i] != 0 ) {
        /* Copy over command */
        if ( cmd[i] != '@' ) {
            arr[j] = cmd[i];
            ++j;
        }
        /* Replace instances of '@' with the given parameters */
        else {
            switch ( num ) {
            case 0:
                if ( rep1 == NULL ) { break; }
                strcat(arr, rep1);
                j += strlen(rep1);
                break;
            case 1:
                if ( rep2 == NULL ) { break; }
                strcat(arr, rep2);
                j += strlen(rep2);
                break;
            case 2:
                if ( rep3 == NULL ) { break; }
                strcat(arr, rep3);
                j += strlen(rep3);
                break;
            default:
                break;
            }

            ++num;
        }
        ++i;
    }
}
