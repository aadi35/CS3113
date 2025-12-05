#include "CS3113/Start.h"

/**
* Author: Aadi Narayan
* Assignment: Rise of the AI
* Date due: 2025-11-08, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/



// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 3;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
int gLives = 3;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

enum SessionState {PLAYING, LOST, WON};

LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
Start *gStart = nullptr;
LevelC *gLevelC = nullptr;

Sound hurt;
Sound win;

SessionState mSessionState = PLAYING;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    hurt = LoadSound("assets/game/hurt.wav");
    win = LoadSound("assets/game/power_up.wav");

    gLevelA = new LevelA(ORIGIN, "#682b1fff");
    gLevelB = new LevelB(ORIGIN, "#011627ff");
    gStart = new Start(ORIGIN, "#ffffff");
    gLevelC = new LevelC(ORIGIN, "#074e87ff");

    gLevels.push_back(gStart);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);

    switchToScene(gLevels[0]);

    SetTargetFPS(FPS);
}

void processInput() 
{
    gCurrentScene->getState().protag->resetMovement();

    if      (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) gCurrentScene->getState().protag->moveLeft();
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) gCurrentScene->getState().protag->moveRight();

    if ((IsKeyPressed(KEY_W) || IsKeyDown(KEY_UP)) && 
        gCurrentScene->getState().protag->isCollidingBottom())
    {
        gCurrentScene->getState().protag->jump();
        PlaySound(gCurrentScene->getState().jumpSound);
    }

    if (GetLength(gCurrentScene->getState().protag->getMovement()) > 1.0f) 
        gCurrentScene->getState().protag->normaliseMovement();

    if (gCurrentScene->getState().protag->getPosition().y == 0.0f && IsKeyDown(KEY_ENTER)) switchToScene(gLevels[1]);

    if (IsKeyPressed(KEY_R) && mSessionState != PLAYING){
        mSessionState = PLAYING;
        switchToScene(gLevels[0]);
        gLives = 3;
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
    if (mSessionState == PLAYING){
        float ticks = (float) GetTime();
        float deltaTime = ticks - gPreviousTicks;
        gPreviousTicks  = ticks;

        deltaTime += gTimeAccumulator;

        if (deltaTime < FIXED_TIMESTEP)
        {
            gTimeAccumulator = deltaTime;
            return;
        }

        if (gCurrentScene->getState().protag->isColliding(gCurrentScene->getState().enemy)){
            gLives -= 1;
            gCurrentScene->shutdown();
            gCurrentScene->initialise();
            PlaySound(hurt);
        }
        if (gCurrentScene->getState().protag->getPosition().y > 800.0f){
            gLives -= 1;
            gCurrentScene->shutdown();
            gCurrentScene->initialise();
            PlaySound(hurt);
        }

        if (gCurrentScene->getState().protag->getPosition().x > gCurrentScene->getOrigin().x + 600 &&
    gCurrentScene->getState().protag->getPosition().y > gCurrentScene->getOrigin().y - 300){ mSessionState = WON;
    PlaySound(win);}


        while (deltaTime >= FIXED_TIMESTEP)
        {
            gCurrentScene->update(FIXED_TIMESTEP);
            deltaTime -= FIXED_TIMESTEP;
        }

        if (gLives == 0) mSessionState = LOST;
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCurrentScene->getState().camera);

    gCurrentScene->render();
    

    EndMode2D();
    DrawText(TextFormat("Lives: %i", gLives), 50, 50, 10, GREEN);
    if (mSessionState == LOST) {
        DrawText("GAME OVER!!!\nR - Main Menu", 200, 200, 100, RED);
    }
    if (mSessionState == WON){
        DrawText("YOU WIN!!!\nR - Main Menu", 200, 200, 100, RED);
        
    }
    EndDrawing();
}

void shutdown() 
{
    delete gLevelA;
    delete gLevelB;
    delete gStart;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID > 0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}