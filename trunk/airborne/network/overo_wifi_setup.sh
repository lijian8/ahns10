#!/bin/sh
# overo_wifi_setup.sh
#
# Overo wifi set up
# Setup and start the wifi interface wlan0
# Setup and start the dhcp server
# 	- Overo 192.168.2.2
#
# $Author$
# $Rev$
# $Date$
# $Id$

ifconfig wlan0 down
ifconfig wlan0 inet 192.168.2.2 netmask 255.255.255.0 broadcast 192.168.2.255
iwconfig wlan0 mode ad-hoc essid heliconnect10
ifconfig wlan0 up

cp udhcpd.conf /etc/udhcpd.conf
touch /var/lib/misc/udhcpd.leases
udhcpd /etc/udhcpd.conf
