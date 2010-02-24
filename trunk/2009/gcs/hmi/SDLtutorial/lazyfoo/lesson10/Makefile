CC = g++
CFLAG = -Wall
PROG = lesson10
SRC = lesson10.cpp
LIBS = -lSDL -lSDL_image -lSDL_ttf

all:		$(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG)
