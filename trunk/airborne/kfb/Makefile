# Make file to create the Kalman filter object

CCFLAGS = -Os

obj/kfb.o: src/kfb.c include/kfb.h
	gcc $(CCFLAGS) -c -I include/ src/kfb.c -o obj/kfb.o
clean:
	rm -f obj/*

