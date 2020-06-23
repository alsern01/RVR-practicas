CC=g++
CFLAGS=-g -I. -std=c++11
DEPS = XLDisplay.h Socket.h Game.h
OBJ = XLDisplay.o Socket.o Game.o
LIBS=-lpthread -lm -lX11 -std=c++11

%.o: %.cc $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

all: client server

client: $(OBJ) MirrorClient.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

server: $(OBJ) MirrorServer.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o client server
