# EXTENSIBLE LOGIN MANAGER

## Introduction

The Extensible Login Manager (ELM) is a highly configurable login manager. Users
are able to create their own widgets that could be easily added to the login
manager.

![elm](http://gabegonzalez.me/img/elm/elm.gif "Using ELM to Login")
![elm login](http://gabegonzalez.me/img/elm/login.jpg "Main Login Screen")
![elm xsession 1](http://gabegonzalez.me/img/elm/xsession1.jpg "Choosing X Session")
![elm xsession 2](http://gabegonzalez.me/img/elm/xsession2.jpg "Displaying Path to X Session Executable")
![elm shutdown](http://gabegonzalez.me/img/elm/powermenu.jpg "Power Menu")

## Requirements

The following packages are required:
- ttf-droid
- xcompmgr

## Installation

To install the _Extensible Login Manager_, run the following commands as root:

```
git clone https://github.com/gabeg805/Extensible-Login-Manager
mv Extensible-Login-Manager /etc/X11/elm
cd /etc/X11/elm
make
make install
```

## Uninstall

To uninstall the _Extensible Login Manager_, run the following command as root:

```
make uninstall
```

Note: The current directory containing all the Makefile, README, code, etc. will
be the only remnants of this package.

## Things to Implement

- [ ] Logout command line option.
- [ ] Setup and connect to the org.freedesktop.DisplayManager bus.
- [ ] Research policykit bus.
      ==== AUTHENTICATING FOR org.freedesktop.policykit.exec ====
      Authentication is needed to run `/usr/bin/mtunnel' as the super user
      polkit-agent-helper-1: error response to PolicyKit daemon: GDBus.Error:org.freedesktop.PolicyKit1.Error.Failed: No session for cookie
- [x] Print to stdout/stderr, in addition to logging everything.
- [x] Add images to username/password entries.
- [x] Add Name= and Exec= info to xsessions.c
- [x] Reduce reliance on static globals and pass in variables instead.
- [x] To keep persistence of widgets, malloc and memcpy the memory. Local
  statics are better.
- [x] Create a elm_sys_vstrcpy that takes variable arguments.
