CFLAG = -Wall
INCLUDE = include
LIBS = -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf
PROG = pracSetup

$(PROG):		obj/pracSetup.o obj/main.o
	g++ $(CFLAG) -o $(PROG) obj/pracSetup.o obj/main.o $(LIBS)

obj/pracSetup.o:	src/pracSetup.cpp
	g++ -c $(CFLAG) -I $(INCLUDE) -o obj/pracSetup.o src/pracSetup.cpp

obj/main.o:		src/main.cpp
	g++ -c $(CFLAG) -I $(INCLUDE) -o obj/main.o src/main.cpp


clean:
	rm -f obj/*.o $(PROG)
