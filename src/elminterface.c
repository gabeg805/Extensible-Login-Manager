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
        {display_frame,          550, 230, false},
        {display_datetime,       110, 595, false},
        {display_power_buttons, 1270, 725, false},
        {display_login,          570, 240, true},
        {0,                        0,   0, 0}
    };

    return apps;
}
