// 
// CREATED BY: Gabriel Gonzalez (contact me at gabeg@bu.edu) 
// 
// 
// NAME:
// 
//     OpenDisplay.c
// 
// 
// SYNTAX: 
// 
//     Without a 'main' function, include the header file:
// 
//         #include "../hdr/OpenDisplay.h"
// 
// 
// PURPOSE:
// 
//     Return an open display for an X server to start on.
// 
// 
// KEYWORDS:
// 
//     N/A
// 
// 
// FUNCTIONS:
// 
//     get_open_display - Return an open display for an X server to start on
// 
// 
// FILE STRUCTURE:
// 
//     * Includes and Declares
//     * Get Open X Display
// 
// 
// MODIFICATION HISTORY:
// 	
//     gabeg Aug 06 2014 <> created
// 
//     gabeg Aug 10 2014 <> Updated header
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



// //////////////////////////////
// ///// GET OPEN X DISPLAY ///// 
// //////////////////////////////

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
