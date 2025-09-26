#include "CS3113/cs3113.h"
#include <math.h>

/**
* Author: Aadi Narayan
* Assignment: Simple 2D Scene
* Date due: 2025-09-27, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

// Global Constants
constexpr int   SCREEN_WIDTH  = 1600 / 2,
                SCREEN_HEIGHT = 900 / 2,
                FPS           = 60,
                SIZE          = 200,
                FRAME_LIMIT   = 100;
const float HORSE_CONST_1 = 400;
const float HORSE_CONST_2 = 100;
const float COWBOY_CONST_1 = 200;
const float COWBOY_CONST_2 = 20;
const float LASSO_CONST_1 = 20;
const float LASSO_CONST_2 = 20;
constexpr float MAX_AMP       = 100.0f;


constexpr char    BG_COLOUR[][8] = {"#4a662a", "#5a802f", "#6e9c3a", "#87bf47"};
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
constexpr Vector2 BASE_SIZE   = { (float) SIZE, (float) SIZE };


constexpr char HORSE_FP[] = "assets/game/horse.png";
constexpr char COWBOY_FP[] = "assets/game/Cowboy.png";
constexpr char LASSO_FP[] = "assets/game/lasso.png";

// Global Variables
AppStatus gAppStatus     = RUNNING;
float     gScaleFactor   = SIZE,
          gLassoAngle    = 0.0f,
          gPulseTime     = 0.0f;
Vector2   gHorsePosition      = ORIGIN;
Vector2   gCowboyPosition      = ORIGIN;
Vector2   gLassoPosition      = ORIGIN;
Vector2   gLassoScale         = BASE_SIZE;
float gPreviousTicks = 0.0f;
int gNumTicks = 0;
int gCurrColor =0;

Texture2D gHorseTexture;
Texture2D gCowboyTexture;
Texture2D gLassoTexture;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

// Function Definitions
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "HW1");

    gHorseTexture = LoadTexture(HORSE_FP);
    gCowboyTexture = LoadTexture(COWBOY_FP);
    gLassoTexture = LoadTexture(LASSO_FP);

    SetTargetFPS(FPS);
}

void processInput() 
{
    if (WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    /**
     *  Calculate delta time
     */
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;


    /**
     *  Apply delta time to the time-dependent logic
     */
    gPulseTime += 1.0f * deltaTime;

    gLassoScale = {
        BASE_SIZE.x + MAX_AMP * cos(gPulseTime),
        BASE_SIZE.y + MAX_AMP * cos(gPulseTime)
    };
    
    gHorsePosition = {SCREEN_WIDTH / 2 + HORSE_CONST_1 * cos(gPulseTime), SCREEN_HEIGHT / 2 + HORSE_CONST_2* sin(gPulseTime)};
    gCowboyPosition = {SCREEN_WIDTH / 2 + COWBOY_CONST_1 * cos(gPulseTime), SCREEN_HEIGHT / 2 + COWBOY_CONST_2* sin(gPulseTime*5)};
    gLassoPosition = {gCowboyPosition.x + LASSO_CONST_1 * cos(gPulseTime*5), gCowboyPosition.y + LASSO_CONST_2* sin(gPulseTime*5)};

    gNumTicks += 1;
    if (gNumTicks == 100){
        gNumTicks = 0;
        gCurrColor += 1;
        if (gCurrColor == 4){gCurrColor = 0;}
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR[gCurrColor]));

    /**
     * @todo Design your UV coordinates (i.e. textureArea) so that only one
     * member is being rendered onto the screen.
     */
    Rectangle horse = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gHorseTexture.width),
        static_cast<float>(gHorseTexture.height)
    };
    

    // Destination rectangle – centred on gPosition
    Rectangle horseDestinationArea = {
        gHorsePosition.x,
        gHorsePosition.y,
        static_cast<float>(200),
        static_cast<float>(200)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 objectOrigin = {
        static_cast<float>(200) / 2.0f,
        static_cast<float>(200) / 2.0f
    };

    Rectangle cowboy = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gCowboyTexture.width),
        static_cast<float>(gCowboyTexture.height)
    };
    

    // Destination rectangle – centred on gPosition
    Rectangle cowboyDestinationArea = {
        gCowboyPosition.x,
        gCowboyPosition.y,
        static_cast<float>(200),
        static_cast<float>(200)
    };

    Rectangle lasso = {
        // top-left corner
        0.0f, 0.0f,

        // bottom-right corner (of texture)
        static_cast<float>(gLassoTexture.width),
        static_cast<float>(gLassoTexture.height)
    };
    

    // Destination rectangle – centred on gPosition
    Rectangle lassoDestinationArea = {
        gLassoPosition.x,
        gLassoPosition.y,
        static_cast<float>(gLassoScale.x),
        static_cast<float>(gLassoScale.y)
    };

    // Render the texture on screen
    DrawTexturePro(
        gHorseTexture, 
        horse, 
        horseDestinationArea, 
        objectOrigin, 
        0.0f, 
        WHITE
    );
    DrawTexturePro(
        gCowboyTexture, 
        cowboy, 
        cowboyDestinationArea, 
        objectOrigin, 
        0.0f, 
        WHITE
    );
    
    DrawTexturePro(
        gLassoTexture, 
        lasso, 
        lassoDestinationArea, 
        objectOrigin, 
        gPulseTime * gPulseTime, 
        WHITE
    );

    EndDrawing();
}

void shutdown() { CloseWindow(); }

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}