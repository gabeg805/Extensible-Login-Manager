* Readme is slightly outdated, but generally holds correct information *

===========
What is it?
===========

The Extensible Login Manager (ELM) is a highly configurable login manager I
created for my Arch Linux system. I made it because I thought it'd be a fun
project to do and it really was super fun!

Along the way though it became clear to me that there was a serious lack of
information out there for creating a login manager. The real hard part, and
consequently the content of the missing information, was in authenticating the
user session. A lot of the stuff I found was nearly ten years old; suffice it to
say that it took some doing to figure out how things worked.

To me, creating something on a Linux system should be transparent (albeit
sometimes difficult). When I found out that doing something like authenticating
had hardly any modern examples online, I decided that I would try and make
everything as open and transparent as possible to (hopefully!) help someone out.
I hope this project reaches that person and gives them a little insight!



=============
Documentation
=============

Documentation for the source and header files can be found at the head of each
file. 

A quick program setup is first required which accomplishes the following:

    - Enables the systemd service file (elm.service)
    - Enables the Linux-PAM module (elm)

The program itself works as follows (starting at system bootup):

    - System boots up and all required processes are started
    - Elm service is started (since it was enabled with "make install") after
      the Getty service
    - Elm service starts the login manager
    - ELM starts an X server (everything following needs a
      running X server)
    - Compositing manager is started
    - Background is displayed
    - Elm applications are displayed
    - The user is prompted to enter the password
    - Username/password combination is sent to the "authenticate" function
    - Inside the "authenticate" function, initiate the PAM module conversation
      ("conv" function) between the username and password combination and the
      Elm PAM module
    - Start the designated PAM module ("elm")
    - Set the PAM user
    - Set the PAM tty
    - Authenticate the username/password combination
    - Check if the user account is valid (if it hasn't expired or if 
      there are access restrictions)
    - Establish credentials for the PAM user
    - Set up a user session for the authenticated user
    - Set the PAM environment variables
    - Destroy all the windows and application that were displayed for logging in
    - Execute the login manager provided xinitrc file (./util/xinit/xinitrc)
    - PAM waits in the background until logout...
    - Closes the user session that was opened
    - Deletes the user credentials that were set
    - Ends the PAM transaction

That's about the size of it. I skipped over some stuff on the GUI side of
things, but I think the real tricky stuff was with PAM, so I went more in depth
with that stuff. You can also check out the source files for a more direct
approach!



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

To install the login manager you want to do the following:

    # unzip Gabes-Login-Manager-master.zip
    # mv Gabes-Login-Manager-master /etc/X11/elm
    # make install
    # make

With "make install", the login manager setup is run so that the login manager
starts at bootup; while the final "make" command generates the executable.



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
              Elm service.

    - Screen remains black and nothing shows up.
            * This one requires a more manual approach. Switch to a tty with:

                    Press: Ctrl + Alt + F# (where F# can be F2, for instance)

              then check the logs are see where things aren't running. Often, a
              simple restart solves the problem.

Who knows what gems the logs may hold!



=====
To-Do
=====

Here's a list of things I want to implement, but haven't had the time to do so yet:
    
    - Start Elm over getty reliably
    
    - Change focus policy
    
    - Focus password entry 
    
    - Find a better way to get open tty ports and all user accounts on comp

    - Print to stdout/stderr by default, and log to a file on request

    - Some GTK functions are deprecated. Use a custom style provider and style
      classes for the applications instead.
