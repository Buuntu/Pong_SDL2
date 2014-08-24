pong: pong2.c ; gcc pong2.c -lSDL2 -lSDL2_gfx -lSDL2_ttf -o pong
debug: pong2.c ; gcc pong2.c -g -lSDL2 -lSDL2_gfx -lSDL2_ttf -o pong
clean:  ; rm pong
