# Make file to create the IMU serail library

CCFLAGS = -O2
LDFLAGS = 

imuserialTester: obj/imuserial.o
	gcc $(LDFLAGS) -I $(HELICONNECT) -I include/ obj/imuserial.o -o imuserialTester src/imuserialTester.c

obj/imuserial.o: src/imuserial.c include/imuserial.h
	gcc $(CCFLAGS) -c -I include/ src/imuserial.c -o obj/imuserial.o
clean:
	rm -f obj/* imuserialTester

