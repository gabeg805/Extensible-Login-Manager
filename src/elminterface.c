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
#include "app/powerbuttons.h"
#include "app/login.h"

/* ************************************************************************** */
/* Display the login interface */
ElmAppBuilder * login_interface(void)
{
    static ElmAppBuilder builder[] = {display_frame, display_datetime,
                                      display_power_buttons, display_login,
                                      NULL};
    return builder;
}
