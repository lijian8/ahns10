# $Author: luis $
# $Name:  $
# $Id: Makefile,v 1.1.1.1 2008-05-05 07:07:57 luis Exp $
# $Source: /home/luis/cvsroot/net-api/Makefile,v $
# $Log: Makefile,v $
#

CC = gcc 
LD = $(CC)

LIBDIR = ../udp/

CFLAGS = -Os  
LDFLAGS = -L./ -L$(LIBDIR) -lstate -lm 
LIB = libstate.a

SRCS = src/MCUTestDriver.c
OBJS = $(MCU_INTERFACE)/obj/mcuserial.o obj/MCUTestDriver.o
INC = -I $(HELICONNECT) -I include -I ../udp/ -I $(MCU_INTERFACE)/include

SERVER = MCUServerTestDriver

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
