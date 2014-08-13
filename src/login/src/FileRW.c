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
//     file_write       - Write to a file
// 
//     file_read        - Read a file's contents
// 
//     get_open_display - Search for X display lock files and return a display that 
//                        is not locked
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Write to File
//     * Read File
//     * Get Open X Display
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

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//Declares
void file_write(char *file, char *phrase, char *fmt);
char * file_read(char *file);
char * get_open_display();



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



// //////////////////////////////
// ///// GET OPEN X DISPLAY ///// 
// //////////////////////////////

// Return an open display in the form ':0'
char * get_open_display() {
    
    // Loop through possible displays
    int d;
    char filename[15];
    char display[5];
    
    for ( d=0; d < 10; d++ ) {
        
        // Pieces of the actual file name
        char *xtmp = "/tmp/.X";
        char *xlock = "-lock";
        snprintf(filename, sizeof(filename), "%s%d%s", xtmp, d, xlock);
        
        // Check for file existence
        int result = access(filename, F_OK);
        
        if ( result != 0 ) 
            break;
    }
    
    // Open display
    snprintf(display, sizeof(display), ":%d", d);
    
    // Allocate memory for display output 
    size_t sz = strlen(display);
    char *type = malloc(sz);  
    strncpy(type, display, sz);
    
    return(type);
}
