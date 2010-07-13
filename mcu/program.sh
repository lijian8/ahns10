#!/bin/sh
#
#
#
#

HEX=obj/main.hex:i  # :i for intel hex - only important for read

PORT=/dev/ttyUSB0
MCU_TARGET=m328p    # Atmega328p
PROGRAMMER=stk500v2 

FUSE=               # -u
MEMTYPE=flash       # flash, eeprom, hfuse, lfuse, efuse
OPERATION=w         # r|w|v

avrdude -p $MCU_TARGET -c $PROGRAMMER -P $PORT -U $MEMTYPE:$OPERATION:$HEX
