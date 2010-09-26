# $Author$
# $Name:  $
# $Id$
# $Source: /home/luis/cvsroot/net-api/Makefile,v $
# $Log: Makefile,v $
# Revision 1.1.1.1  2008-05-05 07:07:57  luis
# initial import

CC = gcc 
LD = $(CC)

LIBSTATE = libstate.a 
CONTROLDIR = ../../control
CONTROL = $(CONTROLDIR)/obj/control.o

CFLAGS = -Os
LDFLAGS =  -L./ -lstate -lm
INCDIR = -I $(CONTROLDIR)/include -I $(HELICONNECT) -I $(MCU_INTERFACE)/include -I include/

SRCS = control.c src/client.c src/server.c $(HELICONNECT)/state.c src/udp.c $(HELICONNECT)/primitive_serialisation.c
OBJS = $(CONTROL) obj/client.o obj/server.o $(HELICONNECT)/state.o obj/udp.o $(HELICONNECT)/primitive_serialisation.o

obj/%.o: src/%.c
	$(CC) -O2 -c $< $(CFLAGS) -o $@ $(INCDIR)

$(LIBSTATE): clean $(OBJS)
	ar -cr $(LIBSTATE) $(OBJS)
	ranlib $(LIBSTATE)

$(CONTROL):
	cd $(CONTROLDIR); make

.PHONY: all clean

all:  clean $(CONTROL) $(LIBSTATE)

clean:
	rm -f $(OBJS) client server src/*~ *.a
