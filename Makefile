CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -Wextra -lSDL2 -lSDL2_gfx -lSDL2_ttf -lSDL2_mixer
DEPS=pong2.h
OBJ = pong2.o

%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

pong: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) 

debug: pong2.c 
	$(CC) -o $@ $^ $(CFLAGS) -g 

clean:  
	rm pong pong2.o
