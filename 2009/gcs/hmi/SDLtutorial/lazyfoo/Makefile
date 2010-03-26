CC = g++
CFLAG = -Wall
PROG = AnimateSpeedTape
LIBS = -lSDL -lSDL_image

SRC = SpeedTape.cpp

all: $(PROG)

$(PROG):	$(SRC)
	$(CC) $(CFLAG) -o $(PROG) $(SRC) $(LIBS)

clean:
	rm -f $(PROG) 
