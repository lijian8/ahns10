

CC = gcc
LD = $(CC)
CFLAGS = -c -g -Wall -I include/
LDFLAGS = 

SRCS = src/little_host_serialisation.c src/SerialisationTestDriver.c
HDRS = include/primitive_serialisation.h
OBJS = obj/little_host_serialisation.o obj/SerialisationTestDriver.o

obj/%.o : src/%.c
	$(CC) $(CFLAGS) -o $@ $<

LittleTestDriver: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o LittleTestDriver

.phony: clean

clean :
	rm $(OBJS) LittleTestDriver
