/* *****************************************************************************
 * 
 * Name:    elminterface.c
 * Author:  Gabriel Gonzalez
 * Email:   gabeg@bu.edu
 * License: The MIT License (MIT)
 * 
 * Description: Display the graphical user interface for the Extensible Login
 *              Manager.
 *              
 * Notes: None.
 * 
 * *****************************************************************************
 */

/* Includes */
#include "elminterface.h"
#include "app/datetime.h"
#include "app/frame.h"
#include "app/login.h"
#include "app/powerbuttons.h"

/* ************************************************************************** */
/* Display the login interface */
ElmApp * login_interface(void)
{
    static ElmApp apps[] = {
        {display_datetime,       110, 595, 0},
        {display_power_buttons, 1270, 725, 0},
        {0,                     0, 0, 0}
    };

   /* static ElmAppBuilder builder[] = { */
   /*      /\* {display_frame,         0, 0, 0}, *\/ */
   /*      {display_datetime,      110, 595, 0}, */
   /*      {display_power_buttons, 1270, 725, 0}, */
   /*      /\* {display_login,         0, 0, 1}, *\/ */
   /*      {0,                     0, 0, 0} */
   /*  }; */

    return apps;
}
