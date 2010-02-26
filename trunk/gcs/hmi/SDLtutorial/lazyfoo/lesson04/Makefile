CC = g++
CFLAG = -Wall
SRC = lesson04.cpp
LIBS = -lSDL -lSDL_image -lSDL_mixer -lSDL_ttf
PROG = lesson04

all: $(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG)
