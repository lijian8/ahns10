# Make file to create the arduino serial library
arduserialTester: obj/arduserial.o
	gcc -I include/ obj/arduserial.o -o arduserialTester src/arduserialTester.c

obj/arduserial.o: src/arduserial.c include/arduserial.h
	gcc -c -I include/ src/arduserial.c -o obj/arduserial.o
clean:
	rm -f obj/* arduserialTester

