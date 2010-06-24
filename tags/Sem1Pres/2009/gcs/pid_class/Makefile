test: obj/pid_class.o obj/test.o
	g++ obj/pid_class.o obj/test.o -o test_pidclass

obj/pid_class.o: pid_class.cpp
	g++ -c pid_class.cpp -o obj/pid_class.o

obj/test.o: test_driver.cpp
	g++ -c test_driver.cpp -o obj/test.o
