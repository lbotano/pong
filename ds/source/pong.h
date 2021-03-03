#include <nds.h>
#include <nf_lib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <nds/ndstypes.h>

#define TOP_SCREEN 0
#define BOTTOM_SCREEN 1

#define SPRITESHEET_ID 0

#define SPRITE_SIZE 8

#define BALL_SPRITE_ID 0

#define LEFT_PALETTE_SPRITE1_ID 1
#define LEFT_PALETTE_SPRITE2_ID 2
#define LEFT_PALETTE_SPRITE3_ID 3
#define LEFT_PALETTE_SPRITE4_ID 4

#define RIGHT_PALETTE_SPRITE1_ID 5
#define RIGHT_PALETTE_SPRITE2_ID 6
#define RIGHT_PALETTE_SPRITE3_ID 7
#define RIGHT_PALETTE_SPRITE4_ID 8

#define LEFT_DIGIT_1_SPRITE_ID 9
#define LEFT_DIGIT_2_SPRITE_ID 10
#define RIGHT_DIGIT_1_SPRITE_ID 11
#define RIGHT_DIGIT_2_SPRITE_ID 12

#define PALETTE_SPEED 3

#define BALL_SPEED_INCREASE 0.05

#define GAME_START_DELAY 60

struct Vector2 {
    int x, y;
};

struct Vector2d {
    double x, y;
};

bool gGameStarted = false;

// Delay counter for when the game restarts (in frames)
int gGameStartCounter = 0;

struct Vector2d gBallRealPos = {
    .x = SCREEN_WIDTH / 2 - SPRITE_SIZE / 2,
    .y = SCREEN_HEIGHT / 2 - SPRITE_SIZE / 2
};
struct Vector2 gBallPos = {
    .x = SCREEN_WIDTH / 2 - SPRITE_SIZE / 2,
    .y = SCREEN_HEIGHT / 2 - SPRITE_SIZE / 2
};
struct Vector2 gPalette1Pos = {
    .x = 4,
    .y = SCREEN_HEIGHT / 2 - SPRITE_SIZE * 4 / 2
};
struct Vector2 gPalette2Pos = {
    .x = SCREEN_WIDTH - SPRITE_SIZE - 4,
    .y = SCREEN_HEIGHT / 2 - SPRITE_SIZE * 4 / 2
}; 

struct Vector2 gLeftDigit1Pos = {
    .x = SCREEN_WIDTH / 2 - SPRITE_SIZE / 2 - 2 * SPRITE_SIZE,
    .y = SCREEN_HEIGHT / 2 - SPRITE_SIZE / 2
};
struct Vector2 gLeftDigit2Pos = {
    .x = SCREEN_WIDTH / 2 - SPRITE_SIZE / 2 - SPRITE_SIZE,
    .y = SCREEN_HEIGHT / 2 - SPRITE_SIZE / 2
};
struct Vector2 gRightDigit1Pos = {
    .x = SCREEN_WIDTH / 2 + SPRITE_SIZE / 2,
    .y = SCREEN_HEIGHT / 2 - SPRITE_SIZE / 2
};
struct Vector2 gRightDigit2Pos = {
    .x = SCREEN_WIDTH / 2 + SPRITE_SIZE / 2 + SPRITE_SIZE,
    .y = SCREEN_HEIGHT / 2 - SPRITE_SIZE / 2
};

double gBallSpeed = 1;
double gBallDirectionX = 0;
double gBallDirectionY = 0;
double gBallUnitVectorDivider = 0;

bool gPalette1GoesUp = false;
bool gPalette1GoesDown = false;

bool gPalette2GoesUp = false;
bool gPalette2GoesDown = false;

uint16 gLeftScore = 0;
uint16 gRightScore = 0;

int main();

void resetGame();

void startGame();

void onBallBounce();

void onScoreChanged();

// Convert a digit from 0 to 9 to its correspondent frame in the spritesheet
uint16 numberToFrame(uint16 number);
