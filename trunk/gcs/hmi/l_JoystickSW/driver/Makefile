# $Author: luis $
# $Name:  $
# $Id: Makefile,v 1.1.1.1 2008-05-05 07:07:05 luis Exp $
# $Source: /home/luis/cvsroot/aerial-simulator/joystick/Makefile,v $
# $Log: Makefile,v $
# Revision 1.1.1.1  2008-05-05 07:07:05  luis
# initial import
#
# Revision 1.1.1.1  2008-05-05 06:45:08  luis
# initial import
#
# Revision 1.1.1.1  2004/03/03 11:03:06  srik
# Initial Release 0.1
#
#

CC = gcc 
LD = $(CC)

CFLAGS =  -c -O2 -Wall

SRCS = heli_joy.c joystick.c 
HDRS = joystick.h
OBJS = heli_joy.o joystick.o
QSRCS =  
QOBJS = 
INCLUDES = -I../include 
LIBSDIR = ../state
LDFLAGS = -L$(LIBSDIR) 
INSTALLDIR = ../bin

all: heli_joy 

heli_joy: $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDES) heli_joy.c
	$(CC) $(CFLAGS) $(INCLUDES) joystick.c
	$(CC) $(LDFLAGS) $(OBJS) -o heli_joy


install:
	make; cp heli_joy query_state $(INSTALLDIR)
clean:
	rm -f $(OBJS) $(QOBJS) heli_joy *~
