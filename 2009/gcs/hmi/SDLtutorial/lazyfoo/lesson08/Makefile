CC = g++
CFLAG = -Wall
LIBS = -lSDL -lSDL_image -lSDL_ttf
SRC = lesson08.cpp
PROG = lesson08

all:		$(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG)
