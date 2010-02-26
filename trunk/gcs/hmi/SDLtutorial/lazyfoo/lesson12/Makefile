INCLUDE = include
CFLAG = -Wall
LIBS = -lSDL -lSDL_image -lSDL_ttf
PROG = lesson12

$(PROG):	obj/lesson12.o obj/main.o
	g++ $(CFLAG) -o $(PROG) obj/lesson12.o obj/main.o $(LIBS)

obj/main.o:	src/main.cpp
	g++ -c $(CFLAG) -I $(INCLUDE) -o obj/main.o src/main.cpp

obj/lesson12.o:	src/lesson12.cpp
	g++ -c $(CFLAG) -I $(INCLUDE) -o obj/lesson12.o src/lesson12.cpp

clean:
	rm -f $(PROG) obj/*.o
