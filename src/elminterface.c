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
        {display_datetime,      ELM_GRAV_BOTTOM_LEFT,  110,  175},
        {display_login,         ELM_GRAV_CENTER,      -100, -120},
        {display_power_buttons, ELM_GRAV_TOP_RIGHT,    40,   20},
        {0,                     ELM_GRAV_NONE,         0,    0}
    };

        /* {display_datetime,      ELM_GRAV_BOTTOM_LEFT, 110,  595}, */
        /* {display_frame,         ELM_GRAV_CENTER,      570,  230}, */
        /* {display_login,         ELM_GRAV_CENTER,      590,  250}, */
        /* {display_power_buttons, ELM_GRAV_TOP_RIGHT,   1320, 20}, */
        /* {0,                     ELM_GRAV_NONE,        0,    0} */

    return apps;
}
