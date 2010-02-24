CC = g++
CFLAG = -Wall
PROG = lesson11
SRC = lesson11.cpp
LIBS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

all:		$(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG)
