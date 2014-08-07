// 
// Created By: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// Name:
// 
//     CommandLine.c
// 
// 
// Syntax: 
// 
//     ./CommandLine
// 
// 
// Purpose:
// 
//     Executes the desired command, and returns the output as a string.
// 
// 
// Keywords:
// 
//     Unknown
// 
// 
// Functions:
// 
//     command_line - Returns command output as a string
// 
// 
// File Structure:
// 
//     * Includes and Declares
//     * Get Command Output
// 
// 
// Modification History:
// 	
//     gabeg Aug 06 2014 <> created
// 
// **********************************************************************************


// /////////////////////////////////
// ///// INCLUDES AND DECLARES /////
// /////////////////////////////////

// Includes
#include "../hdr/CommandLine.h"

#include <string.h>
# include <stdio.h>
#include <stdlib.h>

// Declares
char * command_line(char *cmd);



// //////////////////////////////
// ///// GET COMMAND OUTPUT ///// 
// //////////////////////////////

// Return command output as a string
char * command_line(char *cmd) {
    
    // Max character length of output string
    int outlen = 1024;
    char temp[outlen];
    char output[outlen];
    FILE *fp;
    
    strcpy(output, "");
    
    // Read command output
    fp = popen(cmd, "r");
    while (fgets(temp, sizeof(output)-1, fp) != NULL ) {
        
        // Remove trailing newline characters
        char *pos;
        if ((pos=strchr(temp, '\n')) != NULL)
            *pos = ' ';        
        
        // Append output to sessions string
        strcat(output, temp);
    }
    
    pclose(fp);
    
    
    // Allocate memory for command output 
    size_t sz = strlen(output);
    char *type = malloc(sz);  
    strncpy(type, output, sz);
    
    return(type);
}
