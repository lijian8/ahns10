#!/bin/sh
#
#
#
#

HEX=obj/mcu.hex:i  # :i for intel hex - only important for read

PORT=/dev/ttyUSB0
MCU_TARGET=m328p    # Atmega328p
PROGRAMMER=stk500v2 #avrispmkII #stk500v2 
BAUD=115200

FUSE=               # -u
MEMTYPE=flash       # flash, eeprom, hfuse, lfuse, efuse
OPERATION=w         # r|w|v

avrdude -p $MCU_TARGET -c $PROGRAMMER -P $PORT -b $BAUD -U $MEMTYPE:$OPERATION:$HEX
