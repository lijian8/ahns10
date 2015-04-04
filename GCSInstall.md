# Required Packages #
  * libglut3-dev
  * libqwt5-qt4-dev
  * libqwt4c2
  * libqwtplot3d-qt4-dev

## Installation ##
```
  sudo apt-get install libglut3-dev libqwt5-qt4-dev libqwt4c2 libqwtplot3d-qt4-dev
```

## Required Fix ##
Create symbolic link libqwt.so in /usr/lib/
```
  sudo ln -s /usr/lib/libqwt-qt4.so /usr/lib/libqwt.so
```
Info: https://bugs.launchpad.net/ubuntu/+source/libqwt/+bug/141587

# Other Dependencies #
[Heliconnect10](http://code.google.com/p/heliconnect10/)
Required for udp interface files:
  * state.h
  * state.c
  * commands.h

# Required Environmental Variables #
These need to be added the .profile or .bashrc profile.
Logout to apply or use ` source .profile `

**Heliconnect Path**
```
    export HELICONNECT="${HOME}/heliconnect10/trunk/udp/"
```