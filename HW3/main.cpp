#include "CS3113/Entity.h"

/**
* Author: Aadi Narayan
* Assignment: Lunar Lander
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120;

constexpr char BG_COLOUR[]    = "#0e1b47ff";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float TILE_DIMENSION          = 50.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 981.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f;

enum GameState {PLAYING, WON, LOST};

// Global Variables
AppStatus gAppStatus   = RUNNING;
GameState gGameState = PLAYING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f,
      gFuel = 10.0f;

Entity *gSpaceship = nullptr;
Entity *gLandingPad = nullptr;
Entity *gFuelGauge = nullptr;


// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar lander");

    /*
        ----------- PROTAGONIST -----------
    */

    float sizeRatio  = 48.0f / 64.0f;

    gSpaceship = new Entity(
        {ORIGIN.x, 0.0f}, // position
        {50.0f * sizeRatio, 50.0f},  // scale
        "assets/spaceship.png"         // texture file address
    ); 

    gSpaceship->setColliderDimensions({
        gSpaceship->getScale().x / 3.0f,
        gSpaceship->getScale().y / 3.0f
    });
    gSpaceship->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY/50});

    /*
        ----------- LANDING PAD -----------
    */
    
    gLandingPad = new Entity(
        {ORIGIN.x, SCREEN_HEIGHT - 100},
        {100, 20},
        "assets/landingpad.png"
    );
    gLandingPad->setEntityType(OTHER);
    
        gFuelGauge = new Entity(
        {50, ORIGIN.y},
        {20, gFuel * 50.0f},
        "assets/Solid_white.png"
    );
    
    SetTargetFPS(FPS);
}

void processInput() 
{
    gSpaceship->resetMovement();

    if      (IsKeyDown(KEY_A) && gFuel > 0) gSpaceship->accLeft();
    else if (IsKeyDown(KEY_D) && gFuel > 0) gSpaceship->accRight();



    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    if (gGameState == PLAYING){
        // Delta time
        float ticks = (float) GetTime();
        float deltaTime = ticks - gPreviousTicks;
        gPreviousTicks  = ticks;

        // Fixed timestep
        deltaTime += gTimeAccumulator;

        if (deltaTime < FIXED_TIMESTEP)
        {
            gTimeAccumulator = deltaTime;
            return;
        }

        if (cos(ticks) < 0) gLandingPad->moveLeft();
        else gLandingPad->moveRight();


        while (deltaTime >= FIXED_TIMESTEP)
        {
            gSpaceship->update(FIXED_TIMESTEP, gLandingPad, 1);
            gLandingPad->update(FIXED_TIMESTEP, nullptr, 0);
            

            deltaTime -= FIXED_TIMESTEP;
        }
        
        if (gSpaceship->getAcceleration().x != 0) gFuel -= deltaTime;
        if (gSpaceship->getPosition().y > SCREEN_HEIGHT || gSpaceship->getPosition().x < 0 || gSpaceship->getPosition().x > SCREEN_WIDTH) gGameState = LOST;
        if (gSpaceship->getVelocity().y == 0) gGameState = WON;

        
        gFuelGauge->setScale({20, gFuel * 50.0f});
    }
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    gSpaceship->render();
    gLandingPad->render();
    gFuelGauge->render();

    DrawText(TextFormat("Fuel: %.2f", gFuel), 200, 80, 20, RED);
    
    if (gGameState == LOST) DrawText("GAME OVER\nYOU LOST!", 500, 250, 50, RED);
    if (gGameState == WON) DrawText("YOU WIN!!", 500, 250, 50, RED);

    EndDrawing();
}

void shutdown() 
{ 
    CloseWindow();
}

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