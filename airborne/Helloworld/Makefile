# Makefile for building with the OE cross tools 
#
# OVEROTOP is normally ${HOME}/overo-oe 
#
# OETMP is the same as TMPDIR as defined in ${OVEROTOP}/build/conf/site.conf
#

OETMP = ${OVEROTOP}/tmp

TOOLDIR = ${OETMP}/cross/armv7a/bin

STAGEDIR = ${OETMP}/staging/armv7a-angstrom-linux-gnueabi/usr

CC = ${TOOLDIR}/arm-angstrom-linux-gnueabi-gcc

CFLAGS = -Wall

# DEBUG: GCC COMPILE
#STAGEDIR = .
#CC = gcc

LIBDIR = ${STAGEDIR}/lib

INCDIR = ${STAGEDIR}/include 
 
TARGET = helloworld

OBJS = obj/Helloworld.o 

${TARGET} : $(OBJS)
	${CC} ${CFLAGS} ${OBJS} -L ${LIBDIR} -o ${TARGET} -lm

obj/Helloworld.o: src/Helloworld.c 
	${CC} ${CFLAGS} -I ${INCDIR} -c $< -o $@

clean:
	rm -f ${TARGET} ${OBJS} *~

