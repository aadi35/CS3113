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

constexpr char BG_COLOUR[]    = "#C0897E";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 },
                PADDLE_SCALE  = { 10.0f, 70.0f                      };

constexpr float FIXED_TIMESTEP          = 1.0f / 60.0f;

enum NumPlayers {ONE, TWO};
// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;
NumPlayers gNumPlayers = TWO;
int gActivatedBalls = 0;

Entity *gLPad = nullptr;
Entity *gRPad = nullptr;
Entity *gBalls  = nullptr;
Entity *gWalls = nullptr;
Entity *gGoals = nullptr;

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
    
    gLPad = new Entity({15, ORIGIN.y}, PADDLE_SCALE, "assets/Solid_white.png");
    gLPad ->setType(PADDLE);
    gLPad ->setColliderDimensions(PADDLE_SCALE);


    /*
        ----------- PADDLE R -----------
    */

    gRPad = new Entity({ORIGIN.x * 2 - 15, ORIGIN.y}, PADDLE_SCALE, "assets/Solid_white.png");
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
        gBalls[i].deactivate();
        
    }
    /*
        ----------- Walls -----------
    */
   enum Direction {UP, DOWN};
    gWalls = new Entity[2];
    gWalls[UP].setPosition({0,0});
    gWalls[UP].setScale({SCREEN_WIDTH * 2, 1});
    gWalls[UP].setColliderDimensions({SCREEN_WIDTH*2, 1});
    gWalls[UP].setTexture("assets/Solid_white.png");

    gWalls[DOWN].setPosition({0,SCREEN_HEIGHT});
    gWalls[DOWN].setScale({SCREEN_WIDTH * 2, 1});
    gWalls[DOWN].setColliderDimensions({SCREEN_WIDTH*2, 1});
    gWalls[DOWN].setTexture("assets/Solid_white.png");

    gGoals = new Entity[2];
    gGoals[0].setPosition({0,0});
    gGoals[0].setScale({10, SCREEN_HEIGHT * 2});
    gGoals[0].setColliderDimensions({10, SCREEN_HEIGHT*2});
    gGoals[0].setTexture("assets/Solid_white.png");

    gGoals[1].setPosition({SCREEN_WIDTH, 100});
    gGoals[1].setScale({10, SCREEN_HEIGHT * 2});
    gGoals[1].setColliderDimensions({10, SCREEN_HEIGHT*2});
    gGoals[1].setTexture("assets/ball.png");


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
        gBalls[0].setVelocity({50, 50});
        gBalls[1].deactivate();
        gBalls[2].deactivate();
    }
    if (IsKeyPressed(KEY_TWO)) {
        gBalls[0].activate();
        gBalls[0].setVelocity({50, 50});
        gBalls[1].activate();
        gBalls[1].setVelocity({50, 50});
        gBalls[2].deactivate();
    }
    if (IsKeyPressed(KEY_THREE)) {
        gBalls[0].activate();
        gBalls[0].setVelocity({50, 50});
        gBalls[1].activate();
        gBalls[1].setVelocity({50, 50});
        gBalls[2].activate();
        gBalls[2].setVelocity({50, 50});
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
     

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gLPad->update(FIXED_TIMESTEP, gWalls, 2, nullptr, 0);

        gRPad->update(FIXED_TIMESTEP, gWalls, 2, nullptr, 0);

        for (int i = 0; i < NUMBER_BALLS; i++) gBalls[i].update(FIXED_TIMESTEP, gGoals, 2, gWalls, 2);
        
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
    for (int i = 0; i < 2; i++) gWalls[i].render();
    for (int i = 0; i < 2; i++) gGoals[i].render();
    

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