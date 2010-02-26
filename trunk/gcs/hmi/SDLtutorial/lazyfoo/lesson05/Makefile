CC = g++
CFLAG = -Wall
PROG = lesson05
LIBS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer
SRC = lesson05.cpp

all: $(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG)
