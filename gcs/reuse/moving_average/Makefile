tester: obj/ahns_moving_average.o obj/test.o
	g++ -o tester obj/ahns_moving_average.o obj/test.o

obj/ahns_moving_average.o: ahns_moving_average.cpp 
	g++ -c -o obj/ahns_moving_average.o -I ./ ahns_moving_average.cpp

obj/test.o: test.cpp
	g++ -c -o obj/test.o -I./ test.cpp
