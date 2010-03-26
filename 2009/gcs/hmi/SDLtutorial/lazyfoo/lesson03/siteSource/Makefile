CC = g++
CFLAG = -Wall
PROG = lesson03
LIBS = -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer
SRC = lesson03.cpp

all: $(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG)
