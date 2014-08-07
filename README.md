======
README
======

What is it?
-----------

A login manager I created for my Arch Linux system. I made it because I thought it'd
be a fun project to do and indeed it was super fun! But along the way I realized that 
there isn't that much information out there for being able to do this sort of thing. 
To me, for all that, from what I understand, Linux stands for, that's just wrong. A 
lot of the stuff I found was nearly ten years old; suffice it to say that it took
some doing to figure out how things worked. After I figured out how little 
information there was for this, my new goal was that this login manager would help 
someone out that was trying to do something similar. I hope this project helps that
persone out there!


Documentation
-------------

Onto the details of the login manager, I wanted to try to make everything as 
transparent as possible so I included all my source files and separated them by what
their function is. The bulk of this project is mainly C, but there's some Bash mixed 
in for initializing the X server and session. The C portion is for displaying all the
graphical goodies and for authenticating the username/password combination that is
entered. The way the login manager works is as follows:
    
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
        - "Password:" text image is displayed
        - Window manager dropdown menu button is displayed
        - Password entry box is displayed
        - Username default is "gabeg"
        - Once password is entered, the username/password combination is sent to PAM
        - Pluggable Authentication Module (PAM):
                * Initiate the PAM conversation (with the "conv" function)
                * Starts the designated PAM service ("shart")
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


Installation
------------

To install the login manager you want to do the following:
    # 



Contacts
--------

If you have any problems, feel free to email me at 'gabeg@bu.edu'.


Potential Problems
------------------

Executing this program on startup can have some pretty adverse effects, but fear not! 
    
    - On bootup, your screen remains black and nothing shows up
            * To fix this you want to have a bootable USB at hand, then you want to 
              mount the root partition so execute: 
                    # mount /dev/sdX# /mnt
              
              where X is "a, b, c, ..." and # is a number. Then you want to change 
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
    
    - On bootup, the login manager shows, but once I enter my password, the screen 
      remains black.
            * To fix this check the exec command in "Authentication.c" and make sure 
              it's doing what you want. Also, check the logs! 
    
    - Transparency is not working.
            * When this happens, it means that the compositing manager is starting 
              up either too early or too late. It's something I tried to fix but it's
              difficult to guarentee process order with background process. In short,
              it's a constant battle I've had with the compositing manager... *Sigh*
              To MAYBE fix this, make sure you are using the readahead daemon when 
              you bootup, no promises though! To enable the readahead daemon execute:
                    # systemctl enable systemd-readahead-collect.service systemd-readahead-replay.service
              
              It has to learn your boot process so give it a couple reboots to work. 
              If this doesn't work then hopefully at the very least your bootup time 
              will be faster! 
    
Who knows what gems the logs may hold!
