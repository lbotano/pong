#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define RENDER_SCALE 2
#define GAME_WIDTH SCREEN_WIDTH / RENDER_SCALE
#define GAME_HEIGHT SCREEN_HEIGHT / RENDER_SCALE

#define PALETTE_SPEED .2

#define BALL_SPEED_INCREASE 0.005

struct Vector2 {
    double x, y;
};

bool quit = false;

bool gGameStarted = false;

SDL_Event e;

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer = NULL;

Mix_Chunk* gBounceSound = NULL;

SDL_Texture* gTexSpritesheet = NULL;

SDL_Rect gSrcRectBall = {.x = 16, .y = 0, .w = 16, .h = 16};
SDL_Rect gSrcRectPalette = {.x = 0, .y = 0, .w = 16, .h = 80};

SDL_Rect gDestRectBall = {.w = 16, .h = 16};
SDL_Rect gDestRectPalette1 = {.w = 16, .h = 80};
SDL_Rect gDestRectPalette2 = {.w = 16, .h = 80};

// Digit src rects
SDL_Rect gSrcRectDigits[] = {
    {.x = 32, .y = 48, .w = 16, .h = 16},
    {.x = 32, .y = 0, .w = 16, .h = 16},
    {.x = 48, .y = 0, .w = 16, .h = 16},
    {.x = 16, .y = 16, .w = 16, .h = 16},
    {.x = 32, .y = 16, .w = 16, .h = 16},
    {.x = 48, .y = 16, .w = 16, .h = 16},
    {.x = 16, .y = 32, .w = 16, .h = 16},
    {.x = 32, .y = 32, .w = 16, .h = 16},
    {.x = 48, .y = 32, .w = 16, .h = 16},
    {.x = 16, .y = 48, .w = 16, .h = 16}
};

// Digit dest rects
SDL_Rect gDestRectLeftDigit1 = {.x = SCREEN_WIDTH / RENDER_SCALE / 2 - 48, .y = 0, .w = 16, .h = 16};
SDL_Rect gDestRectLeftDigit2 = {.x = SCREEN_WIDTH / RENDER_SCALE / 2 - 32, .y = 0, .w = 16, .h = 16};
SDL_Rect gDestRectRightDigit1 = {.x = SCREEN_WIDTH / RENDER_SCALE / 2 + 16, .y = 0, .w = 16, .h = 16};
SDL_Rect gDestRectRightDigit2 = {.x = SCREEN_WIDTH / RENDER_SCALE / 2 + 32, .y = 0, .w = 16, .h = 16};

SDL_Rect* gSrcRectLeftDigit1 = &gSrcRectDigits[0];
SDL_Rect* gSrcRectLeftDigit2 = &gSrcRectDigits[0];
SDL_Rect* gSrcRectRightDigit1 = &gSrcRectDigits[0];
SDL_Rect* gSrcRectRightDigit2 = &gSrcRectDigits[0];

struct Vector2 gBallPos;
struct Vector2 gPalette1Pos;
struct Vector2 gPalette2Pos; 

double gBallSpeed = .35;
double gBallDirectionX = 1;
double gBallDirectionY = 1;
double gBallUnitVectorDivider = 0;

bool gPalette1GoesUp = false;
bool gPalette1GoesDown = false;

bool gPalette2GoesUp = false;
bool gPalette2GoesDown = false;

int leftScore = 0;
int rightScore = 0;

int main();

bool init();

bool loadMedia();

void update(float deltaTime);

void closeGame();

SDL_Texture* loadTexture(const char* path);

void resetGame();

void onBallBounce();

void onScoreChanged();
