CC=g++
CFLAGS=-g -I. -std=c++11
DEPS = XLDisplay.h Socket.h Game.h
OBJ = MirrorClient.o XLDisplay.o Socket.o Game.o
LIBS=-lpthread -lm -lX11 -std=c++11

%.o: %.cc $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

all: client

client: $(OBJ) MirrorClient.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o client
