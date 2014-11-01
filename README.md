===========
What is it?
===========

This is a login manager I created for my Arch Linux system. I made it because I  
thought it'd be a fun project to do and indeed it was super fun! Along the way though
it became clear to me that there was a serious lack of information out there for 
creating a login manager. The real hard part, and consequently the content of the 
missing information, was in authenticating the user session. A lot of the stuff I 
found was nearly ten years old; suffice it to say that it took some doing to figure 
out how things worked. From what I gather Linux systems stand for, to me this lack of
information was a serious dissapointment. After I made this discovery, my new goal 
with this project was to help someone out that was trying to do something similar. 
I hope this project reaches that person and helps them out!



=============
Documentation
=============

I wanted to try to make everything as transparent as possible so I included all my
source files and separated them by what their function is. The bulk of this project
is C, but there's some Bash mixed in for initializing the X server and session. The 
C portion is for displaying all the graphical goodies and for authenticating the 
username/password combination that is entered. The way the login manager works is 
as follows:
    
    SYSTEM
    ------
        - Boot up, all required processes are started
        - GLM is started after Getty
    
    GLM
    ---
        - An open display is found, display environment variable is set 
        - X server is initialized with on the display with tty7
        - Compositing manager is started (for transparency)
        - Background is displayed
        - Cursor is changed from the cross to the regular cursor
        - Clock is displayed
        - Frame is displayed (the container with the password entry box)
        - 'Password:' text image is displayed
        - Window manager dropdown menu button is displayed
        - Password entry box is displayed
        - Username default is 'gabeg'
        - Once password is entered, the username/password combination is sent to PAM
        - Pluggable Authentication Module (PAM):
                * Initiate the PAM conversation (with the 'conv' function)
                * Starts the designated PAM service ('shart')
                * Sets the PAM user
                * Sets the PAM tty
                * Authenticates the username/password combination
                * Checks if the user account is valid (if it hasn't expired or if 
                  there are access restrictions)
                * Establish credentials for the PAM user
                * Set up a user session for the authenticated user
                * Set the PAM environment variables
                * Destroy all the windows that were displayed for logging in
                * Execute the GLM provided xinitrc file
                * PAM waits in the background until login...
                * Closes the user session that was opened
                * Deletes the user credentials that were set
                * Ends the PAM transaction

To find out specifically what each source file does, check out their headers, and 
the comments in the files themselves.



============
Installation
============

There are a couple of dependencies that need to be fulfilled before the login 
manager works, here's how to get them: 

    # sudo pacman -S ttf-inconsolata
    # sudo pacman -S ttf-dejavu
    # sudo pacman -S xcompmgr
    # sudo pacman -S hsetroot
    # sudo pacman -S xorg-xsetroot
    # sudo pacman -S xdotool
    # sudo pacman -S xorg-xwininfo
    
Also, you'll need to download "pqiv-git" from the AUR!

To install the login manager you want to do the following:
    
    # unzip Gabes-Login-Manager-master.zip
    # mv Gabes-Login-Manager-master /etc/X11/glm
    # cp /etc/X11/glm/src/pam/glm /etc/pam.d/
    # cp /etc/X11/glm/src/systemd/glm.service /usr/lib/systemd/system/
    # systemctl enable glm.service
    # cd /etc/X11/glm/src/login/
    # make

With the last command, the binary generated is the one needed in order to execute the
login manager on startup. 

In case you want to MAKE CHANGES TO SOURCE CODE, to:
    
    - Change variable values inside the '.c' source files
            * I would suggest to only alter the values of the "#define" statements,
              if you change any other variables then it's up to you to keep track of
              the implications (dependency-wise).
    
    - Add/remove a function from any of the '.c' files. 
            * Be sure to add/remove the function in the declaration at the top of the
              '.c' file, as well as in the '.h' header file. After that, see below.
    
    - Rename a file.
            * Be sure to change the names of source, header, and object files (they 
              all depend on one another). 
            
            * Change the '#include' statement at the top of the files that depend on 
              the one you changed. A simple way to check is to look at all the source
              files (.c) for the filename:
                    # grep -i "FILENAME" *.c
            
            * Change the header guard in the header file. This means that if you want
              to rename, say, 'Clock', to 'Timepiece', then in the header file where 
              it says:

                    #ifndef CLOCK_H
                    #define CLOCK_H

               you change it to:

                    #ifndef TIMEPIECE_H
                    #define TIMEPIECE_H

               Once you're done with the final step, see below.

               
After you have done everything you needed to, run the 'make' command. 
Note: Being in the location where the 'Makefile' is, is crucial when running 'make'.



========
Contacts
========

If you have any problems, feel free to email me at 'gabeg@bu.edu'.



==================
Potential Problems
==================

Executing this program on startup can have some pretty adverse effects, but fear not! 

    - Transparency is not working.
            * When this happens, it means that the compositing manager is starting 
              up either too early or too late. To fix this, hit the leaf icon in the
              bottom right of the screen, and hit restart. This will restart the 
              GLM service.
    
    - I enter my password correctly, but it registers as an incorrect login.
            * When this happens it usually means that the USERNAME global variable
              has gotten overwritten by something in the Authenticate.c module. To 
              test a temporary fix, click on the username menu and hit your name,
              and then try logging in. If you tried this or not, it would be good
              to fix the source of the error because this is only temporary. What I 
              would suggest is to insert the following line anywhere in any module
              to find out where exactly the change occurs:
              
                    file_write("/PATH/TO/WHATEVER/FILE", "a+", "%s %s %s\n", "MODULE LOCATION", USERNAME, PASSWORD);
              
              This will print the USERNAME and PASSWORD variables to a log file and 
              then after you gain access to your system, you can check where things 
              were going wrong. Note: The "MODULE LOCATION" is just there so that 
              when it logs to the file, you know which module the print is coming 
              from.
    
    - After I enter my password, the X session is not started, but rather the screen 
      just turns black.
            * This probably means that the xinitrc executable file is not starting 
              the X session correctly. Check the logs! 
    
    - On bootup, your screen remains black and nothing shows up.
            * This is one of the worst problems to have. To fix this you want to have 
              a bootable USB at hand and mount the root partition

                    # mount /dev/sdX# /mnt

              where X is 'a, b, c, ...' and # is a number. Then you want to change 
              root so that you're essentially logged into your system, execute:

                    # arch-chroot /mnt /bin/bash

              Now you're in your system, execute:

                    # systemctl disable glm.service
                    # exit
                    # reboot
            
            * When you reboot, GLM should no longer start up and you should be able 
              to login to your system using Getty. At this point though, the problem 
              for why the screen was black could be anything so you'll have to do 
              some debugging. When the screen turns black though, it'd be good to 
              check what's happening with X when it starts, check the logs!    
    
Who knows what gems the logs may hold!



=====
To-Do
=====

Here's a list of things I want to implement, but haven't had the time to do so yet:
    
    - Start GLM over getty
    
    - Change focus policy
    
    - Focus password entry 
    
    - use fprintf to stderr and __func__ because log messages are good to have!!
        
    - find a better way to get open tty ports and all user accounts on comp