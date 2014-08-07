// 
// Created By: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// Name:
// 
//     OpenDisplay.c
// 
// 
// Syntax: 
// 
//     ./OpenDisplay
// 
// 
// Purpose:
// 
//     Returns an open display for an X server to start on.
// 
// 
// Keywords:
// 
//     Unknown
// 
// 
// Functions:
// 
//     get_open_display - return an open display for an X server to start on
// 
// 
// File Structure:
// 
//     * Includes and Declares
//     * Get Open Display
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
#include "../hdr/OpenDisplay.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Declares
char * get_open_display();



// ////////////////////////////
// ///// GET OPEN DISPLAY ///// 
// ////////////////////////////

// Return an open display in the form ':0'
char * get_open_display() {
    
    // Display variables
    char dval[5];
    char display[5];
    char filename[15];
    
    
    // Loop through possible displays
    int d;
    for ( d=0; d < 10; d++ ) {
        
        // Pieces of the actual file name
        char *xtmp = "/tmp/.X";
        char *xlock = "-lock";
        snprintf(dval, sizeof(dval), "%d", d);
        
        // File name pieced together
        strcpy(filename, "");
        strcat(filename, xtmp);
        strcat(filename, dval);
        strcat(filename, xlock);
        
        // Check for file existence
        int result = access(filename, F_OK);
        if ( result != 0 ) 
            break;
    }
    
    
    // Open display
    strcpy(display, ":");
    strcat(display, dval);
    
    // Allocate memory for display output 
    size_t sz = strlen(display);
    char *type = malloc(sz);  
    strncpy(type, display, sz);
    
    return(type);
}
