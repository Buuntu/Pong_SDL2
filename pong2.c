/*                                                  //
   SUPER Pong (c) 2014
   Written by Gabriel Abud (gabriel.jabud@gmail.com)
   Requires SDL 2.0 and extensions:
   SDL2_ttf and SDL2_gfxPrimitives

   Latest Changes: 08/20/2014                       
//                                                  */

#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SCREEN_WIDTH 840
#define SCREEN_HEIGHT 540
#define BALL_VEL 8
#define PAD_VEL 10
#define PAD_WIDTH 10
#define PAD_HEIGHT 80

typedef int bool;
enum { false, true };
enum { UP, DOWN, RIGHT, LEFT };
enum { SOUTH_WEST, SOUTH_EAST, NORTH_WEST, NORTH_EAST };

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
bool lost(struct paddle *, struct ball *);
bool loadMedia();
void close_program();

SDL_Window * myWindow = NULL;           // Window to render to
SDL_Renderer * myRenderer = NULL;       // Renderer to use
SDL_Surface * myXout = NULL;            // Exit button
TTF_Font * myFont = NULL;               // TTF font pointer
SDL_Surface * loseSurface = NULL;       // Surface for font
SDL_Texture * loseTexture = NULL;       // Texture for font

bool initScr() {
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        // Initialize SDL_ttf
        if (TTF_Init() == -1) {
            printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            success = false;
        }

        // Create window
        myWindow = SDL_CreateWindow("SUPER Pong!", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (myWindow == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else {
            // Create renderer for window
            myRenderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if ( myRenderer == NULL) {
                printf("SDL_image could not initialize! SDL_image Error: %s\n", SDL_GetError());
                success = false;
            }
            else {
                // Initialize renderer color
                SDL_SetRenderDrawColor (myRenderer, 0x00, 0x00, 0x00, 0x00);
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    // Open the font
    myFont = TTF_OpenFont("/usr/share/fonts/truetype/droid/DroidSans.ttf", 120);
    if (myFont == NULL) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }
    else {
        // Render text
        SDL_Color textColor = { 255, 255, 255 };
        loseSurface = TTF_RenderText_Solid(myFont, "You lose...", textColor);

        loseTexture = SDL_CreateTextureFromSurface(myRenderer,loseSurface);
    }

    return success;
}

void move_paddle (struct paddle *gamepad, unsigned char dir) {
    if (dir == DOWN) {
        if ((gamepad->y + gamepad->height + gamepad->vel) > SCREEN_HEIGHT)
            (gamepad->y) = SCREEN_HEIGHT - gamepad->height;
        else
            (gamepad->y) += gamepad->vel;
    }
    else if (dir == UP) {
        if ((gamepad->y) <= gamepad->vel)
            (gamepad->y) = 1;
        else
            (gamepad->y) -= gamepad->vel;
    }
}

void move_ball (struct ball *gameb, struct paddle *gamep) {
    switch(gameb->direction) {
        case SOUTH_WEST:
        if ((gameb->x - gameb->radius - gamep->width - gameb->vel >= 1) &&
            (gameb->y + gameb->radius + gameb->vel <= SCREEN_HEIGHT)) {
            gameb->x -= gameb->vel;
            gameb->y += gameb->vel;
        }
        else if (gameb->x - gameb->radius - gamep->width - gameb->vel < 1) {
            gameb->x = gamep->width + gameb->radius;
            gameb->y += gameb->vel;
            gameb->direction = SOUTH_EAST;
        }
        else if (gameb->y + gameb->radius + gameb->vel > SCREEN_HEIGHT) {
            gameb->y = SCREEN_HEIGHT - gameb->radius;
            gameb->x -= gameb->vel;
            gameb->direction = NORTH_WEST;
        }
        break;

        case SOUTH_EAST:
        if ((gameb->x + gameb->radius + gameb->vel <= SCREEN_WIDTH) &&
            (gameb->y + gameb->radius + gameb->vel <= SCREEN_HEIGHT)) {
            gameb->x += gameb->vel;
            gameb->y += gameb->vel;
        }
        else if (gameb->x + gameb->radius + gameb->vel > SCREEN_WIDTH) {
            gameb->x = SCREEN_WIDTH - gameb->radius;
            gameb->y += gameb->vel;
            gameb->direction = SOUTH_WEST;
        }
        else if (gameb->y + gameb->radius + gameb->vel > SCREEN_HEIGHT) {
            gameb->y = SCREEN_HEIGHT - gameb->radius;
            gameb->x += gameb->vel;
            gameb->direction = NORTH_EAST;
        }
        break;

        case NORTH_WEST:
        if ((gameb->x - gameb->radius - gameb->vel - gamep->width >= 1) &&
            (gameb->y - gameb->radius - gameb->vel >= 1)) {
            gameb->x -= gameb->vel;
            gameb->y -= gameb->vel;
        }
        else if (gameb->x - gameb->radius - gameb->vel - gamep->width < 1) {
            gameb->x = gamep->width + gameb->radius;
            gameb->y -= gameb->vel;
            gameb->direction = NORTH_EAST;
        }
        else if (gameb->y - gameb->radius - gameb->vel < 1) {
            gameb->y = gameb->radius;
            gameb->x -= gameb->vel;
            gameb->direction = SOUTH_WEST;
        }
        break;

        case NORTH_EAST:
        if ((gameb->x + gameb->radius + gameb->vel <= SCREEN_WIDTH) &&
            (gameb->y - gameb->radius - gameb->vel >= 1)) {
            gameb->x += gameb->vel;
            gameb->y -= gameb->vel;
        }
        else if (gameb->x + gameb->radius + gameb->vel > SCREEN_WIDTH) {
            gameb->x = SCREEN_WIDTH - gameb->radius;
            gameb->y -= gameb->vel;
            gameb->direction = NORTH_WEST;
        }
        else if (gameb->y - gameb->radius - gameb->vel < 1) {
            gameb->y = gameb->radius;
            gameb->x += gameb->vel;
            gameb->direction = SOUTH_EAST;
        }
        break;
    }
}

bool lost(struct paddle * gamepad, struct ball * gameball) {
    if (gameball->x == gamepad->width + gameball->radius) {
        if ((gamepad->y > (gameball->y + gameball->radius)) || 
            (gamepad->y + gamepad->height) < (gameball->y - gameball->radius)) {
            gameball->x -= gamepad->width;
            if (gameball->direction == NORTH_EAST || gameball->direction == NORTH_WEST)
                gameball->y -= gamepad->width;
            else
                gameball->y += gamepad->width;
            return true;
        }
    }

    return false;
}

void close_program() {
        myWindow = NULL;
        myRenderer = NULL;
        TTF_CloseFont(myFont);
        TTF_Quit();
        SDL_DestroyWindow(myWindow);
        SDL_DestroyRenderer(myRenderer);
        SDL_DestroyTexture(loseTexture);
        SDL_FreeSurface(loseSurface);
        SDL_Quit();
}

int main(void)
{
    srand(time(NULL));

    if (! initScr()) 
        printf("Failed to initialize screen!\n");
    else if (! loadMedia())
        printf("Failed to load media!\n");
    else {
        struct paddle gamepaddle = { 
            SCREEN_HEIGHT / 2 - 30, 10, 80, PAD_VEL};
        struct paddle oppo_paddle = { 
            SCREEN_HEIGHT / 2 - 40, PAD_WIDTH, PAD_HEIGHT, PAD_VEL};
        struct ball gameball = { 
            SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 , 15, BALL_VEL, SOUTH_WEST};

        bool quit = false;
        bool have_lost = false;
        unsigned int timer = 1;         // To randomize opponent's movements
        unsigned int dir = rand() % 2;  // Direction of opponent's movements

        SDL_Event event;
        const Uint8 * keys = SDL_GetKeyboardState(NULL);

        while (!quit) {
            // Handles events on queue
            while (SDL_PollEvent(&event) != 0) {
                // User requests quit
                if (event.type == SDL_QUIT)
                    quit = true;
            }

            // Move left paddle
            if (keys[SDL_SCANCODE_UP])
                move_paddle(&gamepaddle, UP);
            else if (keys[SDL_SCANCODE_DOWN])
                move_paddle(&gamepaddle, DOWN);

            // Move right paddle
            if (timer % 40 == 0)
                dir = rand() % 2;
            timer++;
            move_paddle(&oppo_paddle, dir);

            // Move ball
            move_ball(&gameball, &gamepaddle);

            // Clear screen:
            SDL_SetRenderDrawColor(myRenderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(myRenderer);

            // Check if you've lost
            if (lost(&gamepaddle, &gameball)) {
                SDL_Rect center = { SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 - 25, 150, 50};
                SDL_RenderCopy(myRenderer, loseTexture, NULL, &center);
                have_lost = true;
                quit = true;
            }

            // Draw left paddle
            SDL_Rect leftPaddle = { 0, gamepaddle.y, gamepaddle.width, gamepaddle.height };
            SDL_SetRenderDrawColor(myRenderer, 0x00, 0xCC, 0x00, 0xFF);
            SDL_RenderFillRect(myRenderer, &leftPaddle);

            // Draw right paddle
            SDL_Rect rightPaddle = { SCREEN_WIDTH - PAD_WIDTH, oppo_paddle.y, oppo_paddle.width, oppo_paddle.height };
            SDL_SetRenderDrawColor(myRenderer, 0x00, 0xCC, 0x00, 0xFF);
            SDL_RenderFillRect(myRenderer, &rightPaddle);

            // Draw circle
            filledCircleRGBA(myRenderer, gameball.x, gameball.y, gameball.radius, 0x00, 0x99, 0xFF, 0xFF);

            // Draw screen:
            SDL_RenderPresent(myRenderer);

            if (have_lost) 
                SDL_Delay(2500);
        }

        close_program();
    }

	return 0;
}
