/**
* Author: Aadi Narayan
* Assignment: Pong Clone
* Date due: 2025-10-13, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120,
              NUMBER_BALLS = 3;

constexpr char BG_COLOUR[]    = "#03e3fc";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                PADDLE_SCALE  = { 60.0f, 140.0f                      };

constexpr float FIXED_TIMESTEP          = 1.0f / 60.0f;

enum NumPlayers {ONE, TWO};
// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;
NumPlayers gNumPlayers = TWO;
int gActivatedBalls = 0;
enum Winner {None, Left, Right};
Winner Winner = None;

Entity *gLPad = nullptr;
Entity *gRPad = nullptr;
Entity *gBalls  = nullptr;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");

    /*
        ----------- PADDLE L -----------
    */
    
    gLPad = new Entity({15, ORIGIN.y}, PADDLE_SCALE, "assets/ship.png");
    gLPad ->setType(PADDLE);
    gLPad ->setColliderDimensions(PADDLE_SCALE);


    /*
        ----------- PADDLE R -----------
    */

    gRPad = new Entity({ORIGIN.x * 2 - 15, ORIGIN.y}, PADDLE_SCALE, "assets/ship.png");
    gRPad ->setType(PADDLE);
    gRPad ->setColliderDimensions(PADDLE_SCALE);
    
    /*
        ----------- Balls -----------
    */

    gBalls = new Entity[NUMBER_BALLS];
    for (int i = 0; i < NUMBER_BALLS; i++){
        gBalls[i].setPosition({ORIGIN.x + 10 * i, ORIGIN.y + 10 * i});
        gBalls[i].setScale({10, 10});
        gBalls[i].setTexture("assets/ball.png");
        gBalls[i].setColliderDimensions({10,10});
        gBalls[i].setVelocity({100, 100});
        gBalls[i].deactivate();
        
    }


    SetTargetFPS(FPS);
}

void processInput() 
{
    gLPad->resetMovement();

    if      (IsKeyDown(KEY_W)) gLPad->moveUp();
    else if (IsKeyDown(KEY_S)) gLPad->moveDown();

    gRPad->resetMovement();
    if (gNumPlayers == TWO){
        if      (IsKeyDown(KEY_UP)) gRPad->moveUp();
        else if (IsKeyDown(KEY_DOWN)) gRPad->moveDown();
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
    if (IsKeyPressed(KEY_P)){ 
        if (gNumPlayers == ONE){
            gNumPlayers = TWO;
        }else {
            gNumPlayers = ONE;
        }
    }
    if (IsKeyPressed(KEY_ONE)) {
        gBalls[0].activate();
        
        gBalls[1].deactivate();
        gBalls[2].deactivate();
    }
    if (IsKeyPressed(KEY_TWO)) {
        gBalls[0].activate();       
        gBalls[1].activate();
        gBalls[2].deactivate();
    }
    if (IsKeyPressed(KEY_THREE)) {
        gBalls[0].activate();
        gBalls[1].activate();
        gBalls[2].activate();

    }

}

void update() 
{
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
    if (gNumPlayers == ONE){
        if (cos(ticks * 2) > 0) gRPad ->moveUp();
        else gRPad->moveDown();
    }
     

    while (deltaTime >= FIXED_TIMESTEP && Winner == None)
    {
        
        gLPad->update(FIXED_TIMESTEP,nullptr, 0, nullptr, 0);

        gRPad->update(FIXED_TIMESTEP, nullptr, 0, nullptr, 0);

        for (int i = 0; i < NUMBER_BALLS; i++){ 
            gBalls[i].update(FIXED_TIMESTEP, gLPad, 1, gRPad, 1);
            if (gBalls[i].getPosition().x < 0) {
                Winner = Right;
            }else if (gBalls[i].getPosition().x > 1000){
                Winner = Left;
            }
        }
        deltaTime -= FIXED_TIMESTEP;
    }

    
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    gLPad->render();
    gRPad->render();
    for (int i = 0; i < NUMBER_BALLS; i++) gBalls[i].render();
    if (Winner == Left) DrawText("LEFT WON!!", 200, 80, 20, RED);
    if (Winner == Right) DrawText("RIGHT WON!!", 200, 80, 20, RED);

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