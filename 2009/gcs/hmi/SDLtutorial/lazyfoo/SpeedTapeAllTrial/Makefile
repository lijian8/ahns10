CC = g++
CFLAG = -Wall
PROG = AnimateSpeedTape
LIBS = -lSDL -lSDL_image
SRCS = SpeedTape.cpp SpeedTape_main.cpp
HDRS = SpeedTape.h
OBJS = SpeedTape.o SpeedTape_main.o

all: $(PROG)

$(PROG): $(SRCS) $(HDRS)
	$(CC) $(CFLAG) SpeedTape.cpp
	$(CC) $(CFLAG) SpeedTape_main.cpp
	$(CC) $(LIBS) $(OBJS) -o $(PROG)

clean:
	rm -f $(PROG) $(OBJS) *~
