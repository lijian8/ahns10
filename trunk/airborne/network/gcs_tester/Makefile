# $Author: luis $
# $Name:  $
# $Id: Makefile,v 1.1.1.1 2008-05-05 07:07:57 luis Exp $
# $Source: /home/luis/cvsroot/net-api/Makefile,v $
# $Log: Makefile,v $
# Revision 1.1.1.1  2008-05-05 07:07:57  luis
# initial import
#
#

CC = gcc 
LD = $(CC)

LIBDIR = ../udp/
CONTROLDIR = ../../control

CFLAGS = -Os  
LDFLAGS = -L./ -L$(LIBDIR) -lstate -lm 
LIB = libstate.a

SRCS = src/GCSTestDriver.c
OBJS = obj/GCSTestDriver.o
INC = -I $(CONTROLDIR)/include -I $(HELICONNECT) -I include -I ../udp/include -I $(MCU_INTERFACE)/include

SERVER = GCSServerTestDriver

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(SERVER): $(OBJS) $(LIB)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(LIB): 
	cd $(LIBDIR); make $(LIB)

.phony: all clean

all:  clean $(LIB) $(SERVER)

clean:
	rm -f obj/* $(LIBDIR)*.a $(SERVER)
