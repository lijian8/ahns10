# Make file to create the Vicon sensor
viconClient: obj/vicon.o
	g++ obj/vicon.o -lglut -lGLU -L. -lViconDataStreamSDK_CPP -o viconClient

obj/vicon.o: src/vicon.cpp
	g++ -Iinclude/ -c -O2 src/vicon.cpp -o obj/vicon.o
clean:
	rm -f obj/* viconClient

