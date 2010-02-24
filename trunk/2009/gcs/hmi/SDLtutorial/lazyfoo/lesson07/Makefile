CC = g++
CFLAG = -Wall
PROG = lesson07
LIBS = -lSDL -lSDL_image -lSDL_ttf
SRC = lesson07.cpp

all: $(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG)
