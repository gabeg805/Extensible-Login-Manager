/* *****************************************************************************
 * 
 * Name:    elm.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display the Extensible Login Manager.
 * 
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elm.h"

/* Globals */
bool PREVIEW   = false;
bool VERBOSE   = false;

/* ************************************************************************** */
/* Display the Extensible Login Manager  */
int main(int argc, char **argv)
{
    ElmLoginManager *manager = elm_new_login_manager(argc, argv);
    return manager->run();
}
