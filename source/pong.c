#include "pong.h"

int main() {
    consoleDemoInit();
    swiWaitForVBlank();

    // Initialize 2d
    NF_Set2D(TOP_SCREEN, 0);
    NF_Set2D(BOTTOM_SCREEN, 0);

    // Initialize filesystem
    NF_SetRootFolder("NITROFS");

    // Initialize sound
    soundEnable();
    NF_InitRawSoundBuffers();
    
    // Load bounce sound
    NF_LoadRawSound("sfx/bounce", 0, 44100, 1);

    // Initialize tiled background
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(TOP_SCREEN);
    NF_InitTiledBgSys(BOTTOM_SCREEN);

    // Initialize sprite system
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(TOP_SCREEN);
    NF_InitSpriteSys(BOTTOM_SCREEN);

    // Load background
    NF_LoadTiledBg("bg/background", "background", 256, 256);
    NF_LoadTiledBg("bg/background_bottom", "background_bottom", 256, 256);

    // Create background
    NF_CreateTiledBg(TOP_SCREEN, 3, "background");
    NF_CreateTiledBg(BOTTOM_SCREEN, 3, "background_bottom");

    // Load spritesheet
    NF_LoadSpriteGfx("sprite/spritesheet", SPRITESHEET_ID, SPRITE_SIZE, SPRITE_SIZE);
        NF_LoadSpritePal("sprite/spritesheet", SPRITESHEET_ID);
        NF_VramSpriteGfx(TOP_SCREEN, SPRITESHEET_ID, 0, false);
        NF_VramSpriteGfx(BOTTOM_SCREEN, SPRITESHEET_ID, 0, false);
        NF_VramSpritePal(TOP_SCREEN, SPRITESHEET_ID, 0);
        NF_VramSpritePal(BOTTOM_SCREEN, SPRITESHEET_ID, 0);

    // Create ball sprite
    NF_CreateSprite(TOP_SCREEN, BALL_SPRITE_ID, 0, 0, gBallPos.x, gBallPos.y);
    NF_SpriteFrame(TOP_SCREEN, BALL_SPRITE_ID, 1);

    // Create left palette sprites
    NF_CreateSprite(TOP_SCREEN, LEFT_PALETTE_SPRITE1_ID, 0, 0, gPalette1Pos.x, gPalette1Pos.y);
    NF_SpriteFrame(TOP_SCREEN, LEFT_PALETTE_SPRITE1_ID, 0);
     
    NF_CreateSprite(TOP_SCREEN, LEFT_PALETTE_SPRITE2_ID, 0, 0, gPalette1Pos.x, gPalette1Pos.y + SPRITE_SIZE);
    NF_SpriteFrame(TOP_SCREEN, LEFT_PALETTE_SPRITE2_ID, 4);

    NF_CreateSprite(TOP_SCREEN, LEFT_PALETTE_SPRITE3_ID, 0, 0, gPalette1Pos.x, gPalette1Pos.y + 2 * SPRITE_SIZE);
    NF_SpriteFrame(TOP_SCREEN, LEFT_PALETTE_SPRITE3_ID, 4);

    NF_CreateSprite(TOP_SCREEN, LEFT_PALETTE_SPRITE4_ID, 0, 0, gPalette1Pos.x, gPalette1Pos.y + 3 * SPRITE_SIZE);
    NF_SpriteFrame(TOP_SCREEN, LEFT_PALETTE_SPRITE4_ID, 16);

    // Create right palette sprites
    NF_CreateSprite(TOP_SCREEN, RIGHT_PALETTE_SPRITE1_ID, 0, 0, gPalette2Pos.x, gPalette2Pos.y);
    NF_SpriteFrame(TOP_SCREEN, RIGHT_PALETTE_SPRITE1_ID, 0);

    NF_CreateSprite(TOP_SCREEN, RIGHT_PALETTE_SPRITE2_ID, 0, 0, gPalette2Pos.x, gPalette2Pos.y + SPRITE_SIZE);
    NF_SpriteFrame(TOP_SCREEN, RIGHT_PALETTE_SPRITE2_ID, 4);

    NF_CreateSprite(TOP_SCREEN, RIGHT_PALETTE_SPRITE3_ID, 0, 0, gPalette2Pos.x, gPalette2Pos.y + 2 * SPRITE_SIZE);
    NF_SpriteFrame(TOP_SCREEN, RIGHT_PALETTE_SPRITE3_ID, 4);

    NF_CreateSprite(TOP_SCREEN, RIGHT_PALETTE_SPRITE4_ID, 0, 0, gPalette2Pos.x, gPalette2Pos.y + 3 * SPRITE_SIZE);
    NF_SpriteFrame(TOP_SCREEN, RIGHT_PALETTE_SPRITE4_ID, 16);

    // Create digit sprites
    NF_CreateSprite(BOTTOM_SCREEN, LEFT_DIGIT_1_SPRITE_ID, 0, 0, gLeftDigit1Pos.x, gLeftDigit1Pos.y); 
    NF_CreateSprite(BOTTOM_SCREEN, LEFT_DIGIT_2_SPRITE_ID, 0, 0, gLeftDigit2Pos.x, gLeftDigit2Pos.y); 
    NF_CreateSprite(BOTTOM_SCREEN, RIGHT_DIGIT_1_SPRITE_ID, 0, 0, gRightDigit1Pos.x, gRightDigit1Pos.y); 
    NF_CreateSprite(BOTTOM_SCREEN, RIGHT_DIGIT_2_SPRITE_ID, 0, 0, gRightDigit2Pos.x, gRightDigit2Pos.y); 

    // Initialize variables and such
    resetGame();

    while (1) {
        swiDelay(1000);
        NF_SpriteOamSet(TOP_SCREEN);
        NF_SpriteOamSet(BOTTOM_SCREEN);

        // Scan keys
        scanKeys();
        int newPress = keysHeld();

        // Move left palette
        gPalette1GoesUp = newPress & KEY_UP;
        gPalette1GoesDown = newPress & KEY_DOWN;

        // Read controls for right palette
        gPalette2GoesUp = newPress & (KEY_X | KEY_A);
        gPalette2GoesDown = newPress & KEY_B;

        //
        // Move palettes
        //
        
        // Detect left palette movement
        if (gPalette1GoesUp) {
            if (gGameStarted || gGameStartCounter >= GAME_START_DELAY) {
                // MOVE
                gPalette1Pos.y -= PALETTE_SPEED;
            }

            if (!gGameStarted && gGameStartCounter >= GAME_START_DELAY) {
                // INIT
                gBallDirectionX = -1;
                gGameStartCounter = 0;
                gGameStarted = true;
            }
        }

        if (gPalette1GoesDown) {
            if (gGameStarted || gGameStartCounter >= GAME_START_DELAY) {
                // MOVE
                gPalette1Pos.y += PALETTE_SPEED;
            }

            if (!gGameStarted && gGameStartCounter >= GAME_START_DELAY) {
                // INIT
                gBallDirectionX = -1;
                gGameStartCounter = 0;
                gGameStarted = true;
            }
        }

        // Prevent the left palette from getting out of the screen
        if (gPalette1Pos.y < 0)
            gPalette1Pos.y = 0;
        if (gPalette1Pos.y > SCREEN_HEIGHT - 4 * SPRITE_SIZE)
            gPalette1Pos.y = SCREEN_HEIGHT - 4 * SPRITE_SIZE;
        
        // Move left palette sprites
        NF_MoveSprite(TOP_SCREEN, LEFT_PALETTE_SPRITE1_ID, gPalette1Pos.x, gPalette1Pos.y);
        NF_MoveSprite(TOP_SCREEN, LEFT_PALETTE_SPRITE2_ID, gPalette1Pos.x, gPalette1Pos.y + SPRITE_SIZE);
        NF_MoveSprite(TOP_SCREEN, LEFT_PALETTE_SPRITE3_ID, gPalette1Pos.x, gPalette1Pos.y + 2 * SPRITE_SIZE);
        NF_MoveSprite(TOP_SCREEN, LEFT_PALETTE_SPRITE4_ID, gPalette1Pos.x, gPalette1Pos.y + 3 * SPRITE_SIZE);

        // Detect right palette movement
        if (gPalette2GoesUp) {
            if (gGameStarted || gGameStartCounter >= GAME_START_DELAY) {
                // MOVE
                gPalette2Pos.y -= PALETTE_SPEED;
            }

            if (!gGameStarted && gGameStartCounter >= GAME_START_DELAY) {
                // INIT
                gBallDirectionX = 1;
                gGameStartCounter = 0;
                gGameStarted = true;
            }
        }
        if (gPalette2GoesDown) {
            if (gGameStarted || gGameStartCounter >= GAME_START_DELAY) {
                // MOVE
                gPalette2Pos.y += PALETTE_SPEED;
            }

            if (!gGameStarted && gGameStartCounter >= GAME_START_DELAY) {
                // INIT
                gBallDirectionX = 1;
                gGameStartCounter = 0;
                gGameStarted = true;
            }
        }

        // Prevent right palette from getting out of the screen
        if (gPalette2Pos.y < 0) {
            gPalette2Pos.y = 0;
        }
        if (gPalette2Pos.y > floor(SCREEN_HEIGHT - 4 * SPRITE_SIZE)) {
            gPalette2Pos.y = floor(SCREEN_HEIGHT - 4 * SPRITE_SIZE);
        }

        NF_MoveSprite(TOP_SCREEN, RIGHT_PALETTE_SPRITE1_ID, gPalette2Pos.x, gPalette2Pos.y);
        NF_MoveSprite(TOP_SCREEN, RIGHT_PALETTE_SPRITE2_ID, gPalette2Pos.x, gPalette2Pos.y + SPRITE_SIZE);
        NF_MoveSprite(TOP_SCREEN, RIGHT_PALETTE_SPRITE3_ID, gPalette2Pos.x, gPalette2Pos.y + 2 * SPRITE_SIZE);
        NF_MoveSprite(TOP_SCREEN, RIGHT_PALETTE_SPRITE4_ID, gPalette2Pos.x, gPalette2Pos.y + 3 * SPRITE_SIZE);

        //
        // Move ball
        //
        
        // Bounce with top and bottom of the screen
        if (gBallPos.y <= 0) {
            gBallDirectionY = fabs(gBallDirectionY);
            onBallBounce();
        }
        else if (gBallPos.y >= floor(SCREEN_HEIGHT - SPRITE_SIZE)) {
            gBallDirectionY = -fabs(gBallDirectionY);
            onBallBounce();
        }
        
        // Detect collision with palette 1 (left)
        if (
            gBallPos.x <= gPalette1Pos.x + SPRITE_SIZE &&
            gBallPos.y + SPRITE_SIZE >= gPalette1Pos.y &&
            gBallPos.y <= gPalette1Pos.y + SPRITE_SIZE * 4
        ) {
            gBallSpeed += BALL_SPEED_INCREASE;
            gBallDirectionX = 1;
            gBallDirectionY = (gBallPos.y + SPRITE_SIZE / 2.0 - (gPalette1Pos.y + (SPRITE_SIZE * 4) / 2.0)) / 35;
            onBallBounce();
        }

        // Detect collision with palette 2 (right)
        if (
            gBallPos.x + SPRITE_SIZE >= gPalette2Pos.x &&
            gBallPos.y + SPRITE_SIZE >= gPalette2Pos.y &&
            gBallPos.y <= gPalette2Pos.y + SPRITE_SIZE * 4
        ) {
            gBallSpeed += BALL_SPEED_INCREASE;
            gBallDirectionX = -1;
            gBallDirectionY = (gBallPos.y + SPRITE_SIZE / 2.0 - (gPalette2Pos.y + (SPRITE_SIZE * 4) / 2.0)) / 35;
            onBallBounce();
        }

        // Detect if left lost
        if (gBallRealPos.x <= 0) {
            gRightScore++;
            resetGame();
        }

        // Detect if right lost
        if (gBallRealPos.x >= SCREEN_WIDTH - SPRITE_SIZE) {
            gLeftScore++;
            resetGame();
        }

        // Calculate the unit vector of the ball velocity
        if (gBallDirectionX != 0 && gBallDirectionY != 0)
            gBallUnitVectorDivider = fabs(sqrtf32(pow(gBallDirectionX, 2) + pow(gBallDirectionY, 2)) / (pow(gBallDirectionX, 2) + pow(gBallDirectionY, 2)));
        else
            gBallUnitVectorDivider = 60;

        // Move the ball
        gBallRealPos.x += gBallDirectionX * gBallUnitVectorDivider * gBallSpeed * 0.05;
        gBallRealPos.y += gBallDirectionY * gBallUnitVectorDivider * gBallSpeed * 0.05;

        gBallPos.x = (int)gBallRealPos.x;
        gBallPos.y = (int)gBallRealPos.y;

        NF_MoveSprite(TOP_SCREEN, BALL_SPRITE_ID, gBallPos.x, gBallPos.y);

        // Update game start counter
        if (!gGameStarted && gGameStartCounter < GAME_START_DELAY) gGameStartCounter++;

        swiWaitForVBlank();

        oamUpdate(&oamMain);
        oamUpdate(&oamSub);

        consoleClear();
    }
       return 0;
}

void resetGame() {
    gGameStarted = false;

    // Reset ball position
    gBallRealPos.x = SCREEN_WIDTH / 2 - SPRITE_SIZE / 2;
    gBallRealPos.y = SCREEN_HEIGHT / 2 - SPRITE_SIZE / 2;
    gBallPos.x = (int)gBallRealPos.x;
    gBallPos.y = (int)gBallRealPos.y;

    // Reset ball speed
    gBallSpeed = 1;
    gBallDirectionX = 0;
    gBallDirectionY = 0;
    
    // Reset left palette position
    gPalette1Pos.x = 4;
    gPalette1Pos.y = SCREEN_HEIGHT / 2 - SPRITE_SIZE * 4 / 2;

    // Reset right palette position
    gPalette2Pos.x = SCREEN_WIDTH - 4 - SPRITE_SIZE;
    gPalette2Pos.y = SCREEN_HEIGHT / 2 - SPRITE_SIZE * 4 / 2;

    // Display score
    NF_SpriteFrame(BOTTOM_SCREEN, LEFT_DIGIT_1_SPRITE_ID, numberToFrame(gLeftScore / 10));
    NF_SpriteFrame(BOTTOM_SCREEN, LEFT_DIGIT_2_SPRITE_ID, numberToFrame(gLeftScore % 10));
    NF_SpriteFrame(BOTTOM_SCREEN, RIGHT_DIGIT_1_SPRITE_ID, numberToFrame(gRightScore / 10));
    NF_SpriteFrame(BOTTOM_SCREEN, RIGHT_DIGIT_2_SPRITE_ID, numberToFrame(gRightScore % 10));
}

void onBallBounce() {
    NF_PlayRawSound(0, 127, 64, false, 0);
}

uint16 numberToFrame(uint16 number) {
    switch (number) {
        case 0:
            return 14;
        case 1:
            return 2;
        case 2:
            return 3;
        case 3:
            return 5;
        case 4:
            return 6;
        case 5:
            return 7;
        case 6:
            return 9;
        case 7:
            return 10;
        case 8:
            return 11;
        case 9:
            return 13;
    }
    return 0;
}
