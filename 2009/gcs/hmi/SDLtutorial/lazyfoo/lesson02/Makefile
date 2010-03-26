CC = g++
PROG = lesson02
CFLAG = -Wall
SRC = lesson02.cpp
LIBS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer

all: $(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f lesson02
