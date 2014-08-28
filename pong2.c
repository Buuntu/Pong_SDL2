/*                                                  //
   SUPER Pong (c) 2014
   Written by Gabriel Abud (gabriel.jabud@gmail.com)
   Requires SDL 2.0 and extensions:
   SDL2_ttf and SDL2_gfxPrimitives

   Latest Changes: 08/27/2014                       
//                                                  */

#include "pong2.h"

bool initScr() {
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        // Initialize SDL_ttf
        if (TTF_Init() == -1) {
            printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            success = false;
        }

        if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
            success = false;
        }

        // Create window
        myWindow = SDL_CreateWindow("PONG 2.0", SDL_WINDOWPOS_UNDEFINED,
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
        SDL_Color textColor = { 0x99, 0x99, 0x66 };
        mySurface = TTF_RenderText_Solid(myFont, "You lose...", textColor);
        loseTexture = SDL_CreateTextureFromSurface(myRenderer,mySurface);
        mySurface = TTF_RenderText_Solid(myFont, "You win!", textColor);
        winTexture = SDL_CreateTextureFromSurface(myRenderer,mySurface);
        mySurface = TTF_RenderText_Solid(myFont, "Player 1 wins!", textColor);
        player1Wins = SDL_CreateTextureFromSurface(myRenderer, mySurface);
        mySurface = TTF_RenderText_Solid(myFont, "Player 2 wins!", textColor);
        player2Wins = SDL_CreateTextureFromSurface(myRenderer, mySurface);

        // Render logo
        SDL_Color logoTextColor = { 0x00, 0xCC, 0x00 };
        mySurface = TTF_RenderText_Solid(logoFont, "PONG 2.0", logoTextColor);
        logoTexture = SDL_CreateTextureFromSurface(myRenderer, mySurface);

        // Option 1
        SDL_Color menuTextColor = { 0x99, 0x99, 0x66 };
        mySurface = TTF_RenderText_Solid(menuFont, "Single Player", menuTextColor);
        option1Texture = SDL_CreateTextureFromSurface(myRenderer, mySurface);
        // Option 2
        mySurface = TTF_RenderText_Solid(menuFont, "Multiplayer", menuTextColor);
        option2Texture = SDL_CreateTextureFromSurface(myRenderer, mySurface);
        // Option 3
        mySurface = TTF_RenderText_Solid(menuFont, "Quit", menuTextColor);
        option3Texture = SDL_CreateTextureFromSurface(myRenderer, mySurface);
        // Scoreboard
        mySurface = TTF_RenderText_Solid(logoFont, "0", menuTextColor);
        score0 = SDL_CreateTextureFromSurface(myRenderer, mySurface);
        mySurface = TTF_RenderText_Solid(logoFont, "1", menuTextColor);
        score1 = SDL_CreateTextureFromSurface(myRenderer, mySurface);
        mySurface = TTF_RenderText_Solid(logoFont, "2", menuTextColor);
        score2 = SDL_CreateTextureFromSurface(myRenderer, mySurface);
        mySurface = TTF_RenderText_Solid(logoFont, "3", menuTextColor);
        score3 = SDL_CreateTextureFromSurface(myRenderer, mySurface);

    }

    // Load music
    scrollSound = Mix_LoadWAV(scroll_sound);
    enterSound = Mix_LoadWAV(enter_sound);
    paddleSound = Mix_LoadWAV(hit_sound);
    typingSound = Mix_LoadWAV(type_sound);
    if (scrollSound == NULL || enterSound == NULL || paddleSound == NULL || typingSound == NULL) {
        printf("Failed to load sound effects! SDL_mixer Error: %s\n", Mix_GetError());
        success = false;
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
            if (gameball->direction ==  NORTH_EAST) {
                gameball->y -= gamepad->width;
                return true;
            }
            else if (gameball->direction == SOUTH_EAST) {
                gameball->y += gamepad->width;
                return true;
            }
        }
    }

    return false;
}

bool won(struct paddle * gamepad, struct ball * gameball) {
    if (gameball->x == SCREEN_WIDTH - (gamepad->width + gameball->radius)) {
        if ((gamepad->y > (gameball->y + gameball->radius)) ||
            (gamepad->y + gamepad->height) < (gameball->y - gameball->radius)) {
            gameball->x += gamepad->width;
            if (gameball->direction == NORTH_WEST) {
                gameball->y -= gamepad->width;
                return true;
            }
            else if (gameball->direction == SOUTH_WEST) {
                gameball->y += gamepad->width;
                return true;
            }
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
        // Free pointers
        myWindow = NULL;
        myRenderer = NULL;
        myFont = NULL;
        logoFont = NULL;
        loseTexture = NULL;
        logoTexture = NULL;
        option1Texture = NULL;
        option2Texture = NULL;
        option3Texture = NULL;
        score0 = NULL;
        score1 = NULL;
        score2 = NULL;
        score3 = NULL;
        scrollSound = NULL;
        enterSound = NULL;
        paddleSound = NULL;

        // Free SDL_ttf objects
        TTF_CloseFont(myFont);
        TTF_CloseFont(logoFont);
        TTF_Quit();

        // Free SDL objects
        SDL_DestroyWindow(myWindow);
        SDL_DestroyRenderer(myRenderer);
        SDL_DestroyTexture(loseTexture);
        SDL_DestroyTexture(logoTexture);
        SDL_DestroyTexture(option1Texture);
        SDL_DestroyTexture(option2Texture);
        SDL_DestroyTexture(option3Texture);
        SDL_DestroyTexture(score0);
        SDL_DestroyTexture(score1);
        SDL_DestroyTexture(score2);
        SDL_DestroyTexture(score3);
        SDL_FreeSurface(mySurface);

        Mix_Quit();
        SDL_Quit();
}

bool welcome_screen() {
    SDL_Event event;

    bool quit = false;
    bool not_exit = true;
    unsigned int menu_option = 0;

    start_animation(&not_exit, &quit);

    while (!quit) {
        // Handle events
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
                not_exit = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (menu_option == 0) {
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        quit = true;
                        Mix_PlayChannel(-1, enterSound, 0);
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN) {
                        menu_option = 1;
                        Mix_PlayChannel(-1, scrollSound, 0);
                    }
                }
                else if (menu_option == 1) {
                    if (event.key.keysym.sym == SDLK_UP) {
                        menu_option = 0;
                        Mix_PlayChannel(-1, scrollSound, 0);
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN) {
                        menu_option = 2;
                        Mix_PlayChannel(-1, scrollSound, 0);
                    }
                    else if (event.key.keysym.sym == SDLK_RETURN) {
                        multiplayer = true;
                        quit = true;
                        Mix_PlayChannel(-1, enterSound, 0);
                    }
                }
                else if (menu_option == 2) {
                    if (event.key.keysym.sym == SDLK_UP) {
                        menu_option = 1;
                        Mix_PlayChannel(-1, scrollSound, 0);
                    }
                    else if (event.key.keysym.sym == SDLK_RETURN) {
                        quit = true;
                        not_exit = false;
                        Mix_PlayChannel(-1, enterSound, 0);
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

        // 'Multiplayer' option
        SDL_Rect option2Rect = { SCREEN_WIDTH / 2 - 130, SCREEN_HEIGHT / 2 + 80, 260, 40 };
        SDL_Rect menuHighlight2 = { SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT / 2 + 80, 340, 40};
        if (menu_option == 1)
            SDL_RenderFillRect(myRenderer, &menuHighlight2);
        SDL_RenderCopy(myRenderer, option2Texture, NULL, &option2Rect);

        // 'Quit' option
        SDL_Rect option3Rect = { SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 140, 100, 40 };
        SDL_Rect menuHighlight3 = { SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT / 2 + 140, 340, 40};
        if (menu_option == 2)
            SDL_RenderFillRect(myRenderer, &menuHighlight3);
        SDL_RenderCopy(myRenderer, option3Texture, NULL, &option3Rect);

        // Draw screen:
        SDL_RenderPresent(myRenderer);
    }

    return not_exit;
}

void start_animation(bool * not_exit, bool * quit) {

    SDL_Event event;

    unsigned int startTime = SDL_GetTicks();

    bool end = false;
    bool sound_played[5] = { false, false, false, false, false };

    while (! end) {
        // Handles exiting 
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                *quit = true;
                *not_exit = false;
                end = true;
            }
            else if (event.key.keysym.sym == SDLK_ESCAPE)
                end = true;
        }

        SDL_SetRenderDrawColor(myRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(myRenderer);

        // Display PONG logo
        SDL_Rect topCenter = { SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 4 - 100, 400, 120};
        SDL_RenderCopy(myRenderer, logoTexture, NULL, &topCenter);

        SDL_Rect letters;

        int runningTime = SDL_GetTicks();
        if (runningTime - startTime < 400) {
           SDL_Rect letters = { SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 4 - 100, 400, 120 };
        }
        else if (runningTime - startTime < 800) {
            if (!sound_played[1]) {
                Mix_PlayChannel(-1, typingSound, 0);
                sound_played[1] = true;
            }
            SDL_Rect letters = { SCREEN_WIDTH / 2 - 140, SCREEN_HEIGHT / 4 - 100, 340, 120 };
        }
        else if (runningTime - startTime < 1200) {
            if (!sound_played[2]) {
                Mix_PlayChannel(-1, typingSound, 0);
                sound_played[2] = true;
            }
            SDL_Rect letters = { SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 4 - 100, 280, 120 };
        }
        else if (runningTime - startTime < 1600) {
            if (!sound_played[3]) {
                Mix_PlayChannel(-1, typingSound, 0);
                sound_played[3] = true;
            }

            SDL_Rect letters = { SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT / 4 - 100, 220, 120 };
        }
        else if (runningTime - startTime < 2000) {
            if (!sound_played[4]) {
                Mix_PlayChannel(-1, typingSound, 0);
                sound_played[4] = true;
            }

            SDL_Rect letters = { SCREEN_WIDTH / 2 + 40, SCREEN_HEIGHT / 4 - 100, 160, 120 };
        }
        else if (runningTime - startTime < 2500) {
            if (!sound_played[5]) {
                Mix_PlayChannel(-1, typingSound, 0);
                sound_played[5] = true;
            }

            SDL_Rect letters = { 0, 0, 0, 0 };
        }
        SDL_RenderFillRect(myRenderer, &letters);
        
        SDL_RenderPresent(myRenderer);

        if (runningTime - startTime > 2500)
            end = true;
    }
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
            SCREEN_HEIGHT / 2 - 40, PAD_WIDTH, PAD_HEIGHT, PAD_VEL / 2};
        struct ball gameball = { 
            SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 , 15, BALL_VEL, rand() % 2};

        bool quit = false;
        bool have_lost = false;
        bool have_won = false;
        unsigned int dir;   // Direction of opponent's movements
        bool AI_calculated = false;
        int frameNum = 0;   // To control computer's movements
        int leftScore = 0;
        int rightScore = 0;
        bool someoneScored = false;

        if (!welcome_screen())
            quit = true;

        if (multiplayer)
            oppo_paddle.vel = PAD_VEL;

        SDL_Event event;
        const Uint8 * keys = SDL_GetKeyboardState(NULL);

        unsigned int startTime = SDL_GetTicks();
        unsigned int endTime;
        while (!quit) {
            // Handles events on queue
            while (SDL_PollEvent(&event) != 0) {
                // User requests quit
                if (event.type == SDL_QUIT)
                    quit = true;
            }

            // Check if you've won or lost
            if (lost(&gamepaddle, &gameball)) {
                // Increment score and check if you've lost
                if (++rightScore == 3) {
                    have_lost = true;;
                    endTime = SDL_GetTicks();
                }
                else {
                    startTime = SDL_GetTicks();
                    someoneScored = true;
                }
            }
            else if (won(&oppo_paddle, &gameball)) {
                // Increment score and check if you've won
                if (++leftScore == 3) {
                    have_won = true;
                    endTime = SDL_GetTicks();
                }
                else {
                    startTime = SDL_GetTicks();
                    someoneScored = true;
                }
            }
            // Play sound effects
            else {
                if (gameball.x - gameball.radius == gamepaddle.width)
                    Mix_PlayChannel(-1, paddleSound, 0);
                else if (gameball.x + gameball.radius == SCREEN_WIDTH - oppo_paddle.width)
                    Mix_PlayChannel(-1, paddleSound, 0);
            }


            // Move left paddle
            if (!have_lost && !have_won && (SDL_GetTicks() - startTime > 1500) ) {
                if (keys[SDL_SCANCODE_W])
                    move_paddle(&gamepaddle, UP);
                else if (keys[SDL_SCANCODE_S])
                    move_paddle(&gamepaddle, DOWN);

                if (multiplayer) {  // If it's two player
                    if (keys[SDL_SCANCODE_UP])
                        move_paddle(&oppo_paddle, UP);
                    else if (keys[SDL_SCANCODE_DOWN])
                        move_paddle(&oppo_paddle, DOWN);
                }
                else {              // versus an AI
                    
                    if (gameball.direction < 2) // SOUTH_WEST or NORTH_WEST
                        AI_calculated = false;
                    else if (gameball.direction > 1) { // NE or SE
                        if (!AI_calculated) {
                            dir = calculate_direction(gameball.x, gameball.y, &oppo_paddle, gameball.radius, gameball.direction);

                            AI_calculated = true;
                        }

                        if (gameball.x >= SCREEN_WIDTH / 5) { 
                            if (dir > (oppo_paddle.y + (PAD_HEIGHT / 2)  + BALL_VEL - 1))
                                move_paddle(&oppo_paddle, DOWN);
                            else if (dir < (oppo_paddle.y + (PAD_HEIGHT / 2) - BALL_VEL - 1))
                                move_paddle(&oppo_paddle, UP);
                        }
                    }

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


            SDL_Rect center = { SCREEN_WIDTH / 2 - 125, SCREEN_HEIGHT / 2 - 25, 250, 50};
            SDL_Rect center2 = { SCREEN_WIDTH / 2 -150, SCREEN_HEIGHT / 2 - 25, 300, 50};
            // Draw win or lose message
            if (have_won) {
                if (multiplayer)
                    SDL_RenderCopy(myRenderer, player1Wins, NULL, &center2);
                else
                    SDL_RenderCopy(myRenderer, winTexture, NULL, &center);
            }
            else if (have_lost) {
                if (multiplayer)
                    SDL_RenderCopy(myRenderer, player2Wins, NULL, &center2);
                else
                    SDL_RenderCopy(myRenderer, loseTexture, NULL, &center);
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

            if (someoneScored) {
                if (SDL_GetTicks() - startTime > 200) {
                    gameball.x = SCREEN_WIDTH / 2;
                    gameball.y = SCREEN_HEIGHT / 2;
                    gamepaddle.y = SCREEN_HEIGHT / 2 - (PAD_HEIGHT / 2);
                    oppo_paddle.y = SCREEN_HEIGHT / 2 - (PAD_HEIGHT / 2);
                    someoneScored = false;
                }
            }

            // Draw scoreboard
            SDL_Rect rightScoreRect = { (SCREEN_WIDTH/2) + 10, 20, 20, 50 };
            SDL_Rect leftScoreRect = { (SCREEN_WIDTH/2) - 30, 20, 20, 50 };
            SDL_SetRenderDrawColor(myRenderer, 0x00, 0x33, 0x00, 0x99);
            switch (leftScore) {
                case 0:
                    SDL_RenderCopy(myRenderer, score0, NULL, &leftScoreRect);
                    break;
                case 1:
                    SDL_RenderCopy(myRenderer, score1, NULL, &leftScoreRect);
                    break;
                case 2:
                    SDL_RenderCopy(myRenderer, score2, NULL, &leftScoreRect);
                    break;
                case 3:
                    SDL_RenderCopy(myRenderer, score3, NULL, &leftScoreRect);
                    break;
            }

            switch(rightScore) {
                case 0:
                    SDL_RenderCopy(myRenderer, score0, NULL, &rightScoreRect);
                    break;
                case 1:
                    SDL_RenderCopy(myRenderer, score1, NULL, &rightScoreRect);
                    break;
                case 2:
                    SDL_RenderCopy(myRenderer, score2, NULL, &rightScoreRect);
                    break;
                case 3:
                    SDL_RenderCopy(myRenderer, score3, NULL, &rightScoreRect);
                    break;
            }


            // Draw screen:
            SDL_RenderPresent(myRenderer);

            // Exit if lost and 2.5 seconds have passed
            if ((have_lost || have_won) && (SDL_GetTicks() - endTime > 2500)) 
                quit = true;
        }

        close_program();
    }

	return 0;
}
