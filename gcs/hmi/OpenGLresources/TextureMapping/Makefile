# $Author: cvs $
# $Name:  $
# $Id: Makefile,v 1.2 2006/01/18 15:47:45 cvs Exp $
# $Source: /usr/local/cvsroot/heli-api/Makefile,v $
# $Log: Makefile,v $
# Revision 1.2  2006/01/18 15:47:45  cvs
# *** empty log message ***
#
# Revision 1.1.1.1  2005/11/06 10:56:35  cvs
# initial creeation
#
# Revision 1.1.1.1  2005/10/22 16:44:21  cvs
# initial creation
#
# Revision 1.2  2005/10/18 16:30:20  cvs
# *** empty log message ***
#
# Revision 1.1.1.1  2005/10/15 15:14:06  cvs
# initial project comments
#
# Revision 1.1.1.1  2004/03/03 11:03:06  srik
# Initial Release 0.1
#
#

CC = g++
LD = $(CC)

CFLAGS =  -c -O2 
LDFLAGS =  

SRCS = texture.cpp main.cpp
OBJS = texture.o
all:  texture main

texture : texture.cpp
	$(CC) $(CFLAGS) texture.cpp -o texture.o

main : main.c
	$(CC) $(CFLAGS) main.c `sdl-config --cflags`
	$(CC) $(LDFLAGS) $(OBJS) main.o -lGL -lGLU `sdl-config --cflags --libs` -o main

clean:
	rm -f main main.o texture.o
