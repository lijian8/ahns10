CC = g++
CFLAG = -Wall
LIBS = -lSDL -lSDL_image -lSDL_ttf
PROG = lesson06
SRC = lesson06.cpp

all: $(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG)
