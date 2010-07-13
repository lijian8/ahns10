#!/bin/sh
# overo_wifi_setup.sh
#
# Overo wifi set up
#
# $Author$
# $Rev: 115$
# $Date$
# $Id$

ifconfig wlan0 down
iwconfig wlan0 essid 'heliconnect10'
ifconfig wlan0 up

dhclient wlan0

