# Make file to create the Kalman filter object

CCFLAGS = -O2

obj/kf.o: src/kf.c include/kf.h
	gcc $(CCFLAGS) -c -I include/ src/kf.c -o obj/kf.o
clean:
	rm -f obj/* kfTester

