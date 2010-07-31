# $Author$
# $Name:  $
# $Id$
# $Source: /home/luis/cvsroot/net-api/Makefile,v $
# $Log: Makefile,v $
# Revision 1.1.1.1  2008-05-05 07:07:57  luis
# initial import

CC = gcc 
LD = $(CC)

CFLAGS = 
LDFLAGS =  -L./ -lstate -lm
INCDIR = -I $(HELICONNECT) 

SRCS = control.c client.c server.c $(HELICONNECT)/state.c udp.c $(HELICONNECT)/primitive_serialisation.c
INCLUDES = control.h server.h $(HELICONNECT)/state.h udp.h commands.h $(HELICONNECT)/primitive_serialisation.h
OBJS = control.o client.o server.o $(HELICONNECT)/state.o udp.o $(HELICONNECT)/primitive_serialisation.o
LIBSTATE= libstate.a 
all:  $(LIBSTATE)

%.o: %.c
	$(CC) -O2 -c $< $(CFLAGS) -o $@ $(INCDIR)

$(LIBSTATE): $(OBJS)
	ar -cr $(LIBSTATE) $(OBJS)
	ranlib $(LIBSTATE)

clean:
	rm -f *.o client server *~ *.a
