# Make file to create the range sensor serial library
rangeserialTester: obj/rangeserial.o
	gcc -I include/ obj/rangeserial.o -o rangeserialTester src/rangeserialTester.c

obj/rangeserial.o: src/rangeserial.c include/rangeserial.h
	gcc -c -I include/ src/rangeserial.c -o obj/rangeserial.o
clean:
	rm -f obj/* rangeserialTester

