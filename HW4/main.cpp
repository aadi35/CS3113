#include "CS3113/Start.h"

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

LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
Start *gStart = nullptr;
LevelC *gLevelC = nullptr;

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

    if      (IsKeyDown(KEY_A)) gCurrentScene->getState().protag->moveLeft();
    else if (IsKeyDown(KEY_D)) gCurrentScene->getState().protag->moveRight();

    if (IsKeyPressed(KEY_W) && 
        gCurrentScene->getState().protag->isCollidingBottom())
    {
        gCurrentScene->getState().protag->jump();
        PlaySound(gCurrentScene->getState().jumpSound);
    }

    if (GetLength(gCurrentScene->getState().protag->getMovement()) > 1.0f) 
        gCurrentScene->getState().protag->normaliseMovement();

    if (gCurrentScene->getState().protag->getPosition().y == 0.0f && IsKeyDown(KEY_ENTER)) switchToScene(gLevels[1]);

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update() 
{
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
    }
    if (gCurrentScene->getState().protag->getPosition().y > 800.0f){
        gLives -= 1;
        gCurrentScene->shutdown();
        gCurrentScene->initialise();
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
    }

    //if (gLives == 0) switchToScene();
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCurrentScene->getState().camera);

    gCurrentScene->render();
    

    EndMode2D();
    DrawText(TextFormat("Lives: %i", gLives), 50, 50, 10, GREEN);
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