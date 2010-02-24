CC = g++
CFLAG = -Wall
SRC = lesson09.cpp
PROG = lesson09
LIBS = -lSDL -lSDL_image -lSDL_ttf

all:		$(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG)
