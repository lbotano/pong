#include "pong.h"

int main() {
    uint64_t now = SDL_GetPerformanceCounter();
    uint64_t last = 0;
    double deltaTime = 0;

    if (!init()) {
        printf("Failed to initialize\n");
    } else {
        if (!loadMedia()) {
            printf("Failed to load media\n");
        } else {
            resetGame();
            while (!quit) {
                last = now;
                now = SDL_GetPerformanceCounter();

                while (SDL_PollEvent(&e) != 0) {
                    switch (e.type) {
                        case SDL_QUIT:
                            quit = true;
                            break;
                        case SDL_KEYDOWN:
                            // Decide the ball starting direction
                            // depending of who presses a button first
                            if (!gGameStarted) {
                                switch (e.key.keysym.sym) {
                                    case SDLK_w:
                                    case SDLK_s:
                                        gBallDirectionX = -1;
                                        gGameStarted = true;
                                        break;
                                    case SDLK_UP:
                                    case SDLK_DOWN:
                                        gBallDirectionX = 1;
                                        gGameStarted = true;
                                }
                            }
                    }
                }

                update(deltaTime);

                // Render objects
                SDL_RenderClear(gRenderer);
                SDL_RenderCopy(gRenderer, gTexSpritesheet, &gSrcRectBall, &gDestRectBall);
                SDL_RenderCopy(gRenderer, gTexSpritesheet, &gSrcRectPalette, &gDestRectPalette1);
                SDL_RenderCopy(gRenderer, gTexSpritesheet, &gSrcRectPalette, &gDestRectPalette2);
                SDL_RenderCopy(gRenderer, gTexSpritesheet, gSrcRectLeftDigit1, &gDestRectLeftDigit1);
                SDL_RenderCopy(gRenderer, gTexSpritesheet, gSrcRectLeftDigit2, &gDestRectLeftDigit2);
                SDL_RenderCopy(gRenderer, gTexSpritesheet, gSrcRectRightDigit1, &gDestRectRightDigit1);
                SDL_RenderCopy(gRenderer, gTexSpritesheet, gSrcRectRightDigit2, &gDestRectRightDigit2);
                
                SDL_RenderPresent(gRenderer);

                deltaTime = (double)((now - last) * 1000 / (double)SDL_GetPerformanceFrequency());

                // If the game slows down too much don't count the delta time
                // This is needed for SDL_Delay's to work properly
                if (deltaTime >= 500) deltaTime = 0;
            }
        }
    }

    closeGame();
    return 0;
}

bool init() {
	bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not be initialized: %s\n", SDL_GetError());
        success = false;
    } else {
        gWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            printf("SDL could not create the window: %s\n", SDL_GetError());
            success = false;
        } else {
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            SDL_RenderSetScale(gRenderer, RENDER_SCALE, RENDER_SCALE);

            if (gRenderer == NULL) {
                printf("SDL could not create the renderer: %s\n", SDL_GetError());
                success = false;
            } else {
                SDL_SetRenderDrawColor(gRenderer, 155, 188, 15, 255);
                
                // Initialize SDL_Image
                const int IMG_FLAGS = IMG_INIT_PNG;
                if (!(IMG_Init(IMG_FLAGS) & IMG_FLAGS)) {
                    printf("SDL_image could not be initialized: %s\n", SDL_GetError());
                    success = false;
                }

                // Initialize SDL_Mixer
                if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 2048) < 0) {
                    printf("SDL_mixer could not be initialized: %s\n", SDL_GetError());
                }
            }
        }
    }

    return success;
}

bool loadMedia() {
    bool success = true;

    // Load spritesheet
    gTexSpritesheet = loadTexture("spritesheet.png");
    if (gTexSpritesheet == NULL) {
        printf("Unable to load textures: %s\n", SDL_GetError());
        success = false;
    }

    // Load bounce sound
    gBounceSound = Mix_LoadWAV("bounce.wav");
    if (gBounceSound == NULL) {
        printf("Unable to load sound: %s\n", SDL_GetError());
        success = false;
    }

    return success;
}

void update(float deltaTime) {
    gDestRectBall.x = floor(gBallPos.x);
    gDestRectBall.y = floor(gBallPos.y);

    // Set the movement of the palettes
    const uint8_t* currentKeyStates = SDL_GetKeyboardState(NULL);

    if (currentKeyStates[SDL_SCANCODE_W]) gPalette1Pos.y -= PALETTE_SPEED * deltaTime;
    if (currentKeyStates[SDL_SCANCODE_S]) gPalette1Pos.y += PALETTE_SPEED * deltaTime;

    if (currentKeyStates[SDL_SCANCODE_UP]) gPalette2Pos.y -= PALETTE_SPEED * deltaTime;
    if (currentKeyStates[SDL_SCANCODE_DOWN]) gPalette2Pos.y += PALETTE_SPEED * deltaTime;

    // Limit the palette
    if (gPalette1Pos.y <= 0) gPalette1Pos.y = 0;
    if (gPalette1Pos.y + gSrcRectPalette.h >= GAME_HEIGHT) gPalette1Pos.y = GAME_HEIGHT - gSrcRectPalette.h;

    if (gPalette2Pos.y <= 0) gPalette2Pos.y = 0;
    if (gPalette2Pos.y + gSrcRectPalette.h >= GAME_HEIGHT) gPalette2Pos.y = GAME_HEIGHT - gSrcRectPalette.h;

    // Move the palettes to their positions
    gDestRectPalette1.x = floor(gPalette1Pos.x);
    gDestRectPalette1.y = floor(gPalette1Pos.y);

    gDestRectPalette2.x = floor(gPalette2Pos.x);
    gDestRectPalette2.y = floor(gPalette2Pos.y);
    
    //
    // Make ball collisions
    //
    
    // Detect collision with palette 1 (left)
    if (
        gBallPos.x <= gPalette1Pos.x + gSrcRectPalette.w &&
        gBallPos.y + gSrcRectBall.h >= gPalette1Pos.y &&
        gBallPos.y <= gPalette1Pos.y + gSrcRectPalette.h
    ) {
        gBallSpeed += BALL_SPEED_INCREASE;
        gBallDirectionX = 1;
        gBallDirectionY = (gBallPos.y + (double)gSrcRectBall.h / 2.0 - (gPalette1Pos.y + (double)gSrcRectPalette.h / 2.0)) / 35.0;
        onBallBounce();
    }

    // Detect collision with palette 2 (right)
    if (
        gBallPos.x + gSrcRectBall.w >= gPalette2Pos.x &&
        gBallPos.y + gSrcRectBall.h >= gPalette2Pos.y &&
        gBallPos.y <= gPalette2Pos.y + gSrcRectPalette.h
    ) {
        gBallSpeed += BALL_SPEED_INCREASE;
        gBallDirectionX = -1;
        gBallDirectionY = (gBallPos.y + (double)gSrcRectBall.h / 2.0 - (gPalette2Pos.y + (double)gSrcRectPalette.h / 2.0)) / 35.0;
        onBallBounce();
    }

    // Calculate the unit vector of the ball velocity
    if (gBallDirectionX != 0 && gBallDirectionY != 0)
        gBallUnitVectorDivider = fabs(sqrt(pow(gBallDirectionX, 2) + pow(gBallDirectionY, 2)) / (pow(gBallDirectionX, 2) + pow(gBallDirectionY, 2)));
    else
        gBallUnitVectorDivider = 1;

    // Move the ball
    gBallPos.x += gBallDirectionX * gBallUnitVectorDivider * gBallSpeed * deltaTime;
    gBallPos.y += gBallDirectionY * gBallUnitVectorDivider * gBallSpeed * deltaTime;

    // Detect if someone lost
    if (gBallPos.x <= 0) {
        onBallBounce();
        resetGame();
        SDL_Delay(1000);
        rightScore++;
        onScoreChanged();
    }
    if (gBallPos.x + gDestRectBall.w >= SCREEN_WIDTH / RENDER_SCALE) {
        onBallBounce();
        resetGame();
        SDL_Delay(1000);
        leftScore++;
        onScoreChanged();
    }
    
    // Detect if the ball is bouncing with the ceiling
    if (gBallPos.y <= 0) {
        gBallDirectionY = fabs(gBallDirectionY);
        onBallBounce();
    }
    if (gBallPos.y + gDestRectBall.h >= SCREEN_HEIGHT / RENDER_SCALE) {
        gBallDirectionY = -fabs(gBallDirectionY);
        onBallBounce();
    }
}

void closeGame() {
    SDL_DestroyTexture(gTexSpritesheet);
    gTexSpritesheet = NULL;

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    IMG_Quit();
    SDL_Quit();
}

SDL_Texture* loadTexture(const char* path) {
    SDL_Texture* texture = NULL;

    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        printf("Unable to load image \"%s\": %s\n", path, SDL_GetError());
    } else {
        texture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if (texture == NULL) {
            printf("Unable to create texture from \"%s\": %s\n", path, SDL_GetError());
        }

        SDL_FreeSurface(loadedSurface);
    }

    return texture;
}

void resetGame() {
    gGameStarted = false;

    // Reset ball position
    gBallPos.x = SCREEN_WIDTH / RENDER_SCALE / 2;
    gBallPos.y = SCREEN_HEIGHT / RENDER_SCALE / 2;

    // Reset ball speed
    gBallSpeed = .35;
    gBallDirectionX = 0;
    gBallDirectionY = 0;
    
    // Reset left palette position
    gPalette1Pos.x = 16;
    gPalette1Pos.y = SCREEN_HEIGHT / RENDER_SCALE / 2 - gSrcRectPalette.h / 2;

    // Reset right palette position
    gPalette2Pos.x = SCREEN_WIDTH / RENDER_SCALE - 16 - gSrcRectPalette.w;
    gPalette2Pos.y = SCREEN_HEIGHT / RENDER_SCALE / 2 - gSrcRectPalette.h / 2;
}

void onBallBounce() {
    Mix_PlayChannel(0, gBounceSound, 0);
}

void onScoreChanged() {
    printf("%d | %d\n", leftScore, rightScore);
    
    if (leftScore < 10) {
        gSrcRectLeftDigit1 = &gSrcRectDigits[0];
        gSrcRectLeftDigit2 = &gSrcRectDigits[leftScore];
    } else {
        gSrcRectLeftDigit1 = &gSrcRectDigits[leftScore / 10];
        gSrcRectLeftDigit2 = &gSrcRectDigits[leftScore % 10];
    }

    if (rightScore < 10) {
        gSrcRectRightDigit1 = &gSrcRectDigits[0];
        gSrcRectRightDigit2 = &gSrcRectDigits[rightScore];
    } else {
        gSrcRectRightDigit1 = &gSrcRectDigits[rightScore / 10];
        gSrcRectRightDigit2 = &gSrcRectDigits[rightScore % 10];
    }
}
