#ifndef PONG2_H
#define PONG2_H

// THis is a test

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SCREEN_WIDTH 840
#define SCREEN_HEIGHT 540
#define BALL_VEL 8
#define PAD_VEL 10
#define PAD_WIDTH 10
#define PAD_HEIGHT 80

char FONT[] = "Fonts/Square.ttf";
char FONT2[] = "Fonts/robotastic.ttf";

char hit_sound[] = "Sounds/hit.wav";
char scroll_sound[] = "Sounds/switch.wav";
char enter_sound[] = "Sounds/enter.wav";
char type_sound[] = "Sounds/keystroke.wav";

typedef int bool;
enum { false, true };
enum { UP, DOWN, RIGHT, LEFT };
enum { SOUTH_WEST, NORTH_WEST, SOUTH_EAST, NORTH_EAST };

struct paddle
{
	int y;	/* yvalue for the paddle */
    int width;
    int height;
    int vel;
};

struct ball 
{
	int x,y;
    int radius;
    int vel;
    int direction;
};

/* Function Prototypes */
void move_paddle(struct paddle *, unsigned char);
void move_ball(struct ball *, struct paddle *);
bool initScr();
bool lost(struct paddle *, struct ball *);  // Check if left player has lost
bool won(struct paddle *, struct ball *);   // Check if left player has won
bool loadMedia();                           // Load all of the media for the game
void close_program();                       // Free all pointers and exit
bool welcome_screen(unsigned int);          // Main menu
void start_animation(bool *, bool *);       // Logo animation
int replay_menu();                     // Menu after win/loss
void restart (struct ball *, struct paddle *, struct paddle *, unsigned int *);  // Reset
int calculate_direction(int, int, struct paddle *, int, int);   // Calculate computer response
static bool multiplayer = false;

SDL_Window * myWindow = NULL;           // Window to render to
SDL_Renderer * myRenderer = NULL;       // Renderer to use
SDL_Surface * myXout = NULL;            // Exit button
TTF_Font * smallerFont = NULL;          // TTF font
TTF_Font * logoFont = NULL;             // Font for game title
TTF_Font * menuFont = NULL;             // Font for menu options
SDL_Surface * mySurface = NULL;         // Surface for font
SDL_Texture * loseTexture = NULL;       // Texture for losing message
SDL_Texture * player1Wins = NULL;       // Texture for player 1 win
SDL_Texture * player2Wins = NULL;       // Texture for player 2 win
SDL_Texture * winTexture = NULL;        // Texture for winning message
SDL_Texture * logoTexture = NULL;       // PONG logo
SDL_Texture * option1Texture = NULL;    // Option 1 (Menu)
SDL_Texture * option2Texture = NULL;    // Option 2 (Menu)
SDL_Texture * option3Texture = NULL;    // Option 3 (Menu)
SDL_Texture * replayTexture = NULL;     // Play again?
SDL_Texture * menuTexture = NULL;       // Main menu?
SDL_Texture * score0 = NULL;            // Scoreboard options
SDL_Texture * score1 = NULL;            
SDL_Texture * score2 = NULL;
SDL_Texture * score3 = NULL;

// Sound effects
Mix_Chunk *scrollSound = NULL;
Mix_Chunk *enterSound = NULL;
Mix_Chunk *paddleSound = NULL;
Mix_Chunk *typingSound = NULL;

#endif
