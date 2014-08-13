// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     FileRW.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the following header:
// 
//         #include "FileRW.c"
// 
// 
// PURPOSE:
// 
//     Allows easy access to reading and writing to files.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     file_write - Write to a file
//     file_read  - Read a file's contents
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Write to File
//     * Read File
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 12 2014 <> created
// 
// **********************************************************************************



// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/FileRW.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//Declares
void file_write(char *file, char *phrase, char *fmt);
char * file_read(char *file);



// /////////////////////////
// ///// WRITE TO FILE /////
// /////////////////////////

// Write to a file
void file_write(char *file, char *phrase, char *fmt) {
    FILE *handle;
    handle = fopen(file, "w");
    fprintf(handle, fmt, phrase);
    fclose(handle);
}



// /////////////////////
// ///// READ FILE /////
// /////////////////////

// Read a file's contents
char * file_read(char *file) {
    char temp[100];
    
    FILE *handle = fopen(file, "r");
    fgets(temp, sizeof(temp), handle);
    fclose(handle);
    
    // Remove trailing newline characters
    char *pos;
    if ((pos=strchr(temp, '\n')) != NULL)
        *pos = '\0';        
    
    // Allocate memory for command output 
    size_t sz = strlen(temp);
    char *type = malloc(sz);  
    strncpy(type, temp, sz);
    
    return(type);
}
