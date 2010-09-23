# $Author: luis $
# $Name:  $
# $Id: Makefile,v 1.1.1.1 2008-05-05 07:07:57 luis Exp $
# $Date$
# 

CC = gcc 
LD = $(CC)

LIBDIR = ../network/udp
LIB = libstate.a
ARDUDIR = ../arduserial
ARDU = $(ARDUDIR)/obj/arduserial.o
IMUDIR = ../imu
IMU = $(IMUDIR)/obj/imuserial.o
KFDIR = ../kfb
KF = $(KFDIR)/obj/kfb.o
MCUDIR = ../mcu_interface
MCU = $(MCUDIR)/obj/mcuserial.o
CONTROLDIR = ../control
CONTROL = $(CONTROLDIR)/obj/control.o

CFLAGS = -Os  
LDFLAGS = -L./ -L$(LIBDIR) -lstate -lm -pthread

SRCS = src/main.c
OBJS = obj/main.o $(ARDU) $(IMU) $(KF) $(MCU) $(CONTROL)
INC = -I $(HELICONNECT) -I include -I $(LIBDIR)/include -I $(ARDUDIR)/include -I $(IMUDIR)/include -I $(KFDIR)/include -I $(MCUDIR)/include -I $(CONTROLDIR)/include

SERVER = main

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(SERVER): $(OBJS) $(LIB)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(LIB): 
	cd $(LIBDIR); make

$(ARDU):
	cd $(ARDUDIR); make

$(IMU):
	cd $(IMUDIR); make

$(KF):
	cd $(KFDIR); make

$(MCU):
	cd $(MCUDIR); make

$(CONTROL):
	cd $(CONTROLDIR); make

.phony: all clean

all:  clean $(ARDU) $(IMU) $(KF) $(MCU) $(CONTROL) $(LIB) $(SERVER)

clean:
	rm -f obj/* $(LIBDIR)/*.a $(SERVER) $(OBJS)

