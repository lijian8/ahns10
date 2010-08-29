# Make file to create the control library

CFLAGS= -O2
HEADERS= include/control.h

MCUDIR = ../mcu_interface/
INC = -I include/ -I $(MCUDIR)/include -I $(HELICONNECT)

controlTester: obj/control.o obj/controlTester.o
	gcc $(CFLAGS) $(INC) $^ -o $@ 

obj/controlTester.o: src/controlTester.c $(HEADERS)
	gcc $(CFLAGS) -c $(INC) $< -o $@

obj/control.o: src/control.c $(HEADERS)
	gcc $(CFLAGS) -c $(INC) $< -o $@

.PHONY : clean

clean:
	rm -f obj/* controlTester

