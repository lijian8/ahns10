## Legacy Architecture ##
  * Qt threads for m:1 Kernal Threads
  * All based on ground computer
  * Split of HMI and AP over UDP linked computers
  * Support of joystick input
  * Instructions for building 2009 AP and HMI code can be found [here](APandHMI.md)
> ![http://ahns10.googlecode.com/svn/wiki/AHNS09_SA.png](http://ahns10.googlecode.com/svn/wiki/AHNS09_SA.png)

---


## Preliminary Design ##
> ![http://ahns10.googlecode.com/svn/wiki/AHNS10_SA.png](http://ahns10.googlecode.com/svn/wiki/AHNS10_SA.png)

### GCS Design Notes ###
  * Qt Objects
  * QThreads
  * Used for data logging
  * Gains and Parameters can be uploaded to airborne computer

### Airborne Design Notes ###
  * Build distro with OpenEmbedded
  * C library to be used is between GNU C (glibc) and micro C (uclibc)
  * Need support for the desired POSIX NTPL thread library
  * Thread priority, main process -> state estimation -> control-> telemetry
  * State estimation and localisation internal interface with AP (AHNS-10-SY-IC-002)
  * Recieves PWM information from UART to MCU to enable command logging in all flight modes
  * Interface Control Document for Airborne/GCS packets (AHNS-10-SY-IC-001)

---
