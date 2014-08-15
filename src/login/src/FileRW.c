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
//     command_line     - Return output of a Linux command
// 
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
//     gabeg Aug 14 2014 <> Modified command_line function to return an array of 
//                          strings instead of just one long string
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
char ** command_line(char *cmd, int size);



// /////////////////////////
// ///// WRITE TO FILE /////
// /////////////////////////

// Write to a file
void file_write(char *file, char *phrase, char *fmt) {
    FILE *handle = fopen(file, "w");
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
    char *contents = malloc(sz);  
    strncpy(contents, temp, sz);
    
    return contents;
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
    
    // Define open display
    snprintf(display, sizeof(display), ":%d", d);
    
    // Allocate memory for display output 
    size_t sz = strlen(display);
    char *opendisp = malloc(sz);  
    strncpy(opendisp, display, sz);
    
    return opendisp;
}



// ////////////////////////////////////
// ///// GET LINUX COMMAND OUTPUT ///// 
// ////////////////////////////////////

// Return command output as a string
char ** command_line(char *cmd, int size) {
    
    // Output arrays
    char **array = (char**)malloc(sizeof(char*)*size);
    char temp[size];
    
    // Read command output
    FILE *fp  = popen(cmd, "r");
    int i = 0;
    
    while (fgets(temp, sizeof(temp)-1, fp) != NULL ) {
        
        // Remove trailing newline characters
        char *pos;
        if ((pos=strchr(temp, '\n')) != NULL)
            *pos = '\0';
        
        // Add string to the array
        array[i] = strdup(temp);
        i++;
    }
    
    pclose(fp);
    
    return array;
}
