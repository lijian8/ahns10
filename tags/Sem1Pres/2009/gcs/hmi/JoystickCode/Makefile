INCLUDE = include
CFLAG = -Wall
PROG = jstestMod

all:			$(PROG)

$(PROG):		obj/jstestMod.o obj/main.o
	g++ $(CFLAG) -I $(INCLUDE) -o $(PROG) obj/jstestMod.o obj/main.o

obj/jstestMod.o:	src/jstestMod.c
	gcc $(CFLAG) -c -I $(INCLUDE) -o obj/jstestMod.o src/jstestMod.c

obj/main.o:		src/main.cpp
	g++ $(CFLAG) -c -I $(INCLUDE) -o obj/main.o src/main.cpp

clean:
	rm -f $(PROG) obj/*.o
