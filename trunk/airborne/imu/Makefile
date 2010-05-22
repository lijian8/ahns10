# Make file to create the IMU serail library
imuserialTester: obj/imuserial.o
	gcc -I include/ obj/imuserial.o -o imuserialTester src/imuserialTester.c

obj/imuserial.o: src/imuserial.c include/imuserial.h
	gcc -c -I include/ src/imuserial.c -o obj/imuserial.o
clean:
	rm -f obj/* imuserialTester

