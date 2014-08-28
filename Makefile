CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -Wextra -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer
DEPS=pong2.h
OBJ=pong2.o
.PHONY: clean debug all

%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

all: pong

pong: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

debug: pong2.c 
	$(CC) -o pong $^ $(CFLAGS) -g 

clean:  
	rm pong pong2.o
