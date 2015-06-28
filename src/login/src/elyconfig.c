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



/* **************************** */
/* ***** READ CONFIG FILE ***** */
/* **************************** */

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
    while ( fgets(line, sizeof(line), stream) != 0 ) {
        /* Strip config line's leading components */
        get_substring(substr, line, sep, num);
        if ( strcmp(substr, key) != 0 )
            continue;
        char *stripped = strip_config_line(line, sep);

        /* Copy data from stripped string to return variable */
        size_t len  = strlen(stripped);
        size_t size = len + 1;
        size_t i;
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
    if ( val == 0 )
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
                if ( rep1 == 0 ) { break; }
                strcat(arr, rep1);
                j += strlen(rep1);
                break;
            case 1:
                if ( rep2 == 0 ) { break; }
                strcat(arr, rep2);
                j += strlen(rep2);
                break;
            case 2:
                if ( rep3 == 0 ) { break; }
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
