

CC=avr-gcc
OBJCOPY=avr-objcopy

CFLAGS= -mmcu=atmega8

groundpulse.hex: obj/g.o
	$(OBJCOPY) -j .text -j .data -O ihex obj/g.o groundpulse.hex

obj/g.o: groundpulse.c
	$(CC) $(CFLAGS) -o obj/g.o -Os groundpulse.c 



clean:
	rm -r -f *.o *.out *.map *.hex *~ obj/*.o obj/*.out obj/*.map
