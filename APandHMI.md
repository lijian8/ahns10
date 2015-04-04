# Required Packages #
  * libglut3-dev
  * libqwt5-qt4-dev
  * libqwt4c2
  * libqwtplot3d-qt4-dev
  * libnewmat10-dev

> ## Installation ##
```
  sudo apt-get install libglut3-dev libqwt5-qt4-dev libqwt4c2 libqwtplot3d-qt4-dev libnewmat10-dev
```

> ### Required Fix ###
    * Create symbolic link libqwt.so in /usr/lib/
```
    sudo ln -s /usr/lib/libqwt-qt4.so /usr/lib/libqwt.so
```
> > _Info:_ https://bugs.launchpad.net/ubuntu/+source/libqwt/+bug/141587

# Build/Install and Run #

> ## AP GUI ##
> Assuming trunk checkout
```
 cd gcs/autopilot
 ./build.sh
 ./autopilot
```

> ## HMI GUI ##
> Assuming trunk checkout
```
 cd gcs/hmi/GUIofficial/
 chmod +x build.sh
 ./build.sh
 ./GUIofficial
```