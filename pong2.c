/*                                                  //
   SUPER Pong (c) 2014
   Written by Gabriel Abud (gabriel.jabud@gmail.com)
   Requires SDL 2.0 and extensions:
   SDL2_ttf and SDL2_gfxPrimitives

   Latest Changes: 08/24/2014                       
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

char FONT[] = "Fonts/Square.ttf";
char FONT2[] = "Fonts/robotastic.ttf";

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
bool lost(struct paddle *, struct ball *);
bool loadMedia();
void close_program();
bool welcome_screen();
int calculate_direction(int, int, struct paddle *, int, int);

SDL_Window * myWindow = NULL;           // Window to render to
SDL_Renderer * myRenderer = NULL;       // Renderer to use
SDL_Surface * myXout = NULL;            // Exit button
TTF_Font * myFont = NULL;               // TTF font pointer
TTF_Font * logoFont = NULL;             // Font for game title
TTF_Font * menuFont = NULL;             // Font for menu options
SDL_Surface * mySurface = NULL;         // Surface for font
SDL_Texture * loseTexture = NULL;       // Texture for font
SDL_Texture * logoTexture = NULL;       // PONG logo
SDL_Texture * option1Texture = NULL;    // Option 1 (Menu)
SDL_Texture * option2Texture = NULL;    // Option 2 (Menu)

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
    myFont = TTF_OpenFont(FONT, 120);
    logoFont = TTF_OpenFont(FONT2, 120);
    menuFont = TTF_OpenFont(FONT, 120);
    if (myFont == NULL || logoFont == NULL) {
        printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        success = false;
    }
    else {
        // Render text
        SDL_Color textColor = { 255, 255, 255 };
        mySurface = TTF_RenderText_Solid(myFont, "You lose...", textColor);
        loseTexture = SDL_CreateTextureFromSurface(myRenderer,mySurface);

        // Render logo
        SDL_Color logoTextColor = { 0x00, 0xCC, 0x00 };
        mySurface = TTF_RenderText_Solid(logoFont, "PONG 2.0", logoTextColor);
        logoTexture = SDL_CreateTextureFromSurface(myRenderer, mySurface);

        // Option 1
        SDL_Color menuTextColor = { 0x99, 0x99, 0x66 };
        mySurface = TTF_RenderText_Solid(menuFont, "Single Player", menuTextColor);
        option1Texture = SDL_CreateTextureFromSurface(myRenderer, mySurface);
        // Option 2
        mySurface = TTF_RenderText_Solid(menuFont, "Quit", menuTextColor);
        option2Texture = SDL_CreateTextureFromSurface(myRenderer, mySurface);
    }

    return success;
}

void move_paddle (struct paddle *gamepad, unsigned char dir) {
    if (dir == DOWN) {
        if ((gamepad->y + gamepad->height + gamepad->vel) > SCREEN_HEIGHT)
            gamepad->y = SCREEN_HEIGHT - gamepad->height;
        else
            gamepad->y += gamepad->vel;
    }
    else if (dir == UP) {
        if (gamepad->y <= gamepad->vel)
            gamepad->y = 1;
        else
            gamepad->y -= gamepad->vel;
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
        if ((gameb->x + gameb->radius + gameb->vel <= SCREEN_WIDTH - gamep->width) &&
            (gameb->y + gameb->radius + gameb->vel <= SCREEN_HEIGHT)) {
            gameb->x += gameb->vel;
            gameb->y += gameb->vel;
        }
        else if (gameb->x + gameb->radius + gameb->vel > SCREEN_WIDTH - gamep->width) {
            gameb->x = SCREEN_WIDTH - gameb->radius - gamep->width;
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
        if ((gameb->x + gameb->radius + gameb->vel <= SCREEN_WIDTH - gamep->width) && (gameb->y - gameb->radius - gameb->vel >= 1)) {
            gameb->x += gameb->vel;
            gameb->y -= gameb->vel;
        }
        else if (gameb->x + gameb->radius + gameb->vel > SCREEN_WIDTH - gamep->width) {
            gameb->x = SCREEN_WIDTH - gameb->radius - gamep->width;
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

int calculate_direction(int ballx, int bally, struct paddle * pad, int radius, int dir) {
    int pad_destination;   // Value to return: y-value of where the paddle should be

    int y = SCREEN_HEIGHT - bally; // y value = -y (coordinate system flipped on y axis)
    int m, b;
    m = (dir == NORTH_EAST) ? 1 : -1;
    if (ballx == 0)
        b = 0;
    else
        b = y - (ballx * m);    // y = mx + b where m = 1

    if (dir == NORTH_EAST) {
        // y-value at x = SCREEN_WIDTH
        int y_edge = m * (SCREEN_WIDTH - radius) + b;
        if (y_edge >= 0 && y_edge <= SCREEN_HEIGHT)
            pad_destination = SCREEN_HEIGHT - y_edge;
        else {
            int new_x = ((SCREEN_HEIGHT - radius) - b ) / m;
            pad_destination = calculate_direction(new_x, radius, pad, radius, SOUTH_EAST);
        }
    }
    else if (dir == SOUTH_EAST) {
        int x_intersect = b * m * -1;
        // If the ball will reach the edge of the screen
        if (x_intersect >= SCREEN_WIDTH - radius) {
            y = m * (SCREEN_WIDTH - radius) + b;
            pad_destination = SCREEN_HEIGHT - y;
        }
        else 
            pad_destination = calculate_direction(x_intersect - radius, SCREEN_HEIGHT - radius, pad, radius, NORTH_EAST);
    }

    return pad_destination;
}

void close_program() {
        myWindow = NULL;
        myRenderer = NULL;
        myFont = NULL;
        logoFont = NULL;
        loseTexture = NULL;
        logoTexture = NULL;
        option1Texture = NULL;
        option2Texture = NULL;

        TTF_CloseFont(myFont);
        TTF_CloseFont(logoFont);
        TTF_Quit();

        SDL_DestroyWindow(myWindow);
        SDL_DestroyRenderer(myRenderer);
        SDL_DestroyTexture(loseTexture);
        SDL_DestroyTexture(logoTexture);
        SDL_DestroyTexture(option1Texture);
        SDL_DestroyTexture(option2Texture);

        SDL_FreeSurface(mySurface);

        SDL_Quit();
}

bool welcome_screen() {
    SDL_Event event;

    bool quit = false;
    bool not_exit = true;
    unsigned int menu_option = 0;

    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
                not_exit = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (menu_option == 0) {
                    if (event.key.keysym.sym == SDLK_RETURN)
                        quit = true;
                    else if (event.key.keysym.sym == SDLK_DOWN)
                        menu_option = 1;
                }
                else if (menu_option == 1) {
                    if (event.key.keysym.sym == SDLK_UP)
                        menu_option = 0;
                    else if (event.key.keysym.sym == SDLK_RETURN) {
                        quit = true;
                        not_exit = false;
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(myRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(myRenderer);

        // Display PONG logo
        SDL_Rect topCenter = { SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 4 - 100, 400, 120};
        SDL_RenderCopy(myRenderer, logoTexture, NULL, &topCenter);

        SDL_SetRenderDrawColor(myRenderer, 0x00, 0x33, 0x00, 0xFF);

        // 'Single Player' option
        SDL_Rect option1Rect = { SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 + 20, 320, 40 };
        SDL_Rect menuHighlight1 = { SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT / 2 + 20, 340, 40 };
        if (menu_option == 0)
            SDL_RenderFillRect(myRenderer, &menuHighlight1);
        SDL_RenderCopy(myRenderer, option1Texture, NULL, &option1Rect);

        // 'Quit' option
        SDL_Rect option2Rect = { SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 80, 100, 40 };
        SDL_Rect menuHighlight2 = { SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT / 2 + 80, 340, 40};
        if (menu_option == 1)
            SDL_RenderFillRect(myRenderer, &menuHighlight2);
        SDL_RenderCopy(myRenderer, option2Texture, NULL, &option2Rect);

        // Draw screen:
        SDL_RenderPresent(myRenderer);
    }

    return not_exit;
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
            SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 , 15, BALL_VEL, rand() % 2};

        bool quit = false;
        bool have_lost = false;
        unsigned int timer = 1;         // To randomize opponent's movements
        unsigned int dir = -1;  // Direction of opponent's movements
        bool AI_calculated = false;

        if (!welcome_screen())
            quit = true;


        SDL_Event event;
        const Uint8 * keys = SDL_GetKeyboardState(NULL);

        unsigned int startTime = SDL_GetTicks();
        unsigned int lossTime;
        while (!quit) {
            // Handles events on queue
            while (SDL_PollEvent(&event) != 0) {
                // User requests quit
                if (event.type == SDL_QUIT)
                    quit = true;
            }

            // Move left paddle
            if (!have_lost && (SDL_GetTicks() - startTime > 1500) ) {
                if (keys[SDL_SCANCODE_UP])
                    move_paddle(&gamepaddle, UP);
                else if (keys[SDL_SCANCODE_DOWN])
                    move_paddle(&gamepaddle, DOWN);

                if (gameball.direction == NORTH_WEST || gameball.direction == SOUTH_WEST)
                    AI_calculated = false;
                else if (gameball.direction == NORTH_EAST || gameball.direction == SOUTH_EAST) {
                    if (!AI_calculated) {
                        dir = calculate_direction(gameball.x, gameball.y, &oppo_paddle, gameball.radius, gameball.direction);

                        AI_calculated = true;
                    }
                    
                    /*printf("dir = %d\n", dir);
                    printf("oppo_paddle.y + BALL_VEL - 1 = %d\n", oppo_paddle.y + BALL_VEL - 1);*/
                    if (dir > (oppo_paddle.y + (PAD_HEIGHT / 2)  + BALL_VEL - 1))
                        move_paddle(&oppo_paddle, DOWN);
                    else if (dir < (oppo_paddle.y + (PAD_HEIGHT / 2) - BALL_VEL - 1))
                        move_paddle(&oppo_paddle, UP);
                }

                // Move ball
                move_ball(&gameball, &gamepaddle);
            }

            // Clear screen:
            SDL_SetRenderDrawColor(myRenderer, 0x00, 0x00, 0x00, 0xFF);
            SDL_RenderClear(myRenderer);

            // Draw vertical line through the middle
            SDL_SetRenderDrawColor(myRenderer, 0x00, 0xCC, 0x00, 0xFF);
            int i;
            for (i = 0; i < SCREEN_HEIGHT ; i++)
                if (i % 30 < 15)
                    SDL_RenderDrawPoint(myRenderer, SCREEN_WIDTH / 2, i);

            // Draw message if you've lost
            if (have_lost || lost(&gamepaddle, &gameball)) {
                SDL_Rect center = { SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT / 2 - 25, 150, 50};
                SDL_RenderCopy(myRenderer, loseTexture, NULL, &center);
                if (!have_lost) {
                    have_lost = true;
                    lossTime = SDL_GetTicks();
                }
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

            // Exit if lost and 2.5 seconds have passed
            if (have_lost && (SDL_GetTicks() - lossTime > 2500)) 
                quit = true;
        }

        close_program();
    }

	return 0;
}
