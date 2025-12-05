#include "CS3113/Bullet.h"
#include "CS3113/LevelC.h"
#include "CS3113/ShaderProgram.h"
#include <deque>

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 2;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

ShaderProgram gShader;

float fireTimerOne = 0.0f;
float fireTimerTwo = 0.0f;
const float FIRE_COOLDOWN = 2.0f;

Vector2 gLightPosition = {0,0};

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};
std::deque<Bullet*> magazine = {};

bool levelThreeFlag = true;
int shaderOn = 0;

enum PlayerState {MOVING, SHOOTING};

PlayerState gPlayerOneState = MOVING;
PlayerState gPlayerTwoState = MOVING;

int gCurrScene = 0;

LevelA *gLevelA = nullptr;
LevelB *gLevelB = nullptr;
LevelC *gLevelC = nullptr;
StartScreen *gStartScreen = nullptr;

Effects *gEffects = new Effects(ORIGIN, SCREEN_WIDTH*100, SCREEN_HEIGHT*100);

Entity *sunglasses = nullptr;

Sound tankMove;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();
void fireBullet(Entity *firingPlayer, Bullet *firingBullet);

void switchToScene(Scene *scene)
{   
    gCurrentScene = scene;
    gCurrentScene->initialise();
    gEffects->start(FADEIN);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();

    gLevelA = new LevelA(ORIGIN, "#d5a035ff");
    gLevelB = new LevelB(ORIGIN, "#136fbbff");
    gLevelC = new LevelC(ORIGIN, "#7a12aeff");
    gStartScreen = new StartScreen(ORIGIN, "#FFFFFFFF");

    gLevels.push_back(gStartScreen);
    gLevels.push_back(gLevelA);
    gLevels.push_back(gLevelB);
    gLevels.push_back(gLevelC);

    switchToScene(gLevels[0]);

    gShader.load("shaders/vertex.glsl", "shaders/fragment.glsl");

    tankMove = LoadSound("assets/game/tankMove.wav");

    for (int i = 0; i < 10; i++){
        Bullet *newBullet = new Bullet({ORIGIN.x - 300, ORIGIN.y}, {25.0f, 25.0f}, "assets/game/Bullet.png");
        newBullet->deactivate();
        magazine.push_back(newBullet);
    }

    sunglasses = new Entity(ORIGIN, {25.0f, 25.0f}, "assets/game/Sunglasses.png", NPC);
    sunglasses->setAIType(WANDERER);
    sunglasses->deactivate();

    SetTargetFPS(FPS);
}

void processInput() 
{
    gCurrentScene->getState().playerOne->resetMovement();
    gCurrentScene->getState().playerTwo->resetMovement();
    if (IsKeyPressed(KEY_E) && gPlayerOneState == MOVING) gPlayerOneState = SHOOTING;
    else if (IsKeyPressed(KEY_E) && gPlayerOneState == SHOOTING) {
        gPlayerOneState = MOVING;
        gCurrentScene->getState().playerOne->setAngle(0.0f);
    }
    if (gPlayerOneState == MOVING){
        if      (IsKeyPressed(KEY_A)) {gCurrentScene->getState().playerOne->moveLeft(); PlaySound(tankMove);}
        else if (IsKeyPressed(KEY_D)) {gCurrentScene->getState().playerOne->moveRight(); PlaySound(tankMove);}
        if      (IsKeyPressed(KEY_W)) {gCurrentScene->getState().playerOne->moveUp(); PlaySound(tankMove);}
        else if (IsKeyPressed(KEY_S)) {gCurrentScene->getState().playerOne->moveDown(); PlaySound(tankMove);}
    }else{
        if (IsKeyDown(KEY_A)) gCurrentScene->getState().playerOne->rotateLeft();
        if (IsKeyDown(KEY_D)) gCurrentScene->getState().playerOne->rotateRight();
        if (IsKeyPressed(KEY_F) && gCurrentScene->getState().playerOne->isActive() && fireTimerOne <= 0.0f){ 
            fireBullet(gCurrentScene->getState().playerOne, magazine.front());
            Bullet* temp = magazine.front();
            magazine.pop_front();
            magazine.push_back(temp);
            fireTimerOne = FIRE_COOLDOWN;
        }
    }

    if (IsKeyPressed(KEY_M) && gPlayerTwoState == MOVING) gPlayerTwoState = SHOOTING;
    else if (IsKeyPressed(KEY_M) && gPlayerTwoState == SHOOTING) {
        gPlayerTwoState = MOVING;
        gCurrentScene->getState().playerTwo->setAngle(0.0f);
    }
    if (gPlayerTwoState == MOVING){
        if      (IsKeyPressed(KEY_LEFT)) {gCurrentScene->getState().playerTwo->moveLeft(); PlaySound(tankMove);}
        else if (IsKeyPressed(KEY_RIGHT)) {gCurrentScene->getState().playerTwo->moveRight(); PlaySound(tankMove);}
        if      (IsKeyPressed(KEY_UP)) {gCurrentScene->getState().playerTwo->moveUp(); PlaySound(tankMove);}
        else if (IsKeyPressed(KEY_DOWN)) {gCurrentScene->getState().playerTwo->moveDown(); PlaySound(tankMove);}
    }else{
        if (IsKeyDown(KEY_LEFT)) gCurrentScene->getState().playerTwo->rotateLeft();
        if (IsKeyDown(KEY_RIGHT)) gCurrentScene->getState().playerTwo->rotateRight();
        if (IsKeyPressed(KEY_L) && gCurrentScene->getState().playerTwo->isActive() && fireTimerTwo <= 0.0f){ 
            fireBullet(gCurrentScene->getState().playerTwo, magazine.front());
            Bullet* temp = magazine.front();
            magazine.pop_front();
            magazine.push_back(temp);
            fireTimerTwo = FIRE_COOLDOWN;
        }
    }

    if ((gCurrentScene->getState().playerOne->getLives() == 0 || gCurrentScene->getState().playerTwo->getLives() == 0 || gCurrScene == 0) 
    && IsKeyDown(KEY_H)){
        if (gCurrScene == 3) gCurrScene = -1;
        switchToScene(gLevels[++gCurrScene]);
        
    }

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

    if (fireTimerOne > 0.0f) {
    fireTimerOne -= deltaTime;
}
    if (fireTimerTwo > 0.0f) {
    fireTimerTwo -= deltaTime;
}

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;
        for (Bullet* bullet : magazine){
            bullet->update(FIXED_TIMESTEP, 
                gCurrentScene->getState().playerOne,
                gCurrentScene->getState().playerTwo, 
                gCurrentScene->getState().map, 
                gCurrentScene->getState().boxes,
                gCurrentScene->getState().numBoxes);
            }
        gEffects->update(FIXED_TIMESTEP, nullptr);
        if (gCurrScene == 3 && levelThreeFlag){ sunglasses->activate(); levelThreeFlag = false;}
        sunglasses->update(FIXED_TIMESTEP,
        gCurrentScene->getState().playerOne,
        gCurrentScene->getState().map,
        gCurrentScene->getState().playerTwo
        );
        if (sunglasses->collidedWith == gCurrentScene->getState().playerOne){
            gLightPosition = gCurrentScene->getState().playerTwo->getPosition();
            shaderOn = 1;
        }
        if (sunglasses->collidedWith == gCurrentScene->getState().playerTwo){
            gLightPosition = gCurrentScene->getState().playerOne->getPosition();
            shaderOn = 1;
        }
    }
    sunglasses->resetMovement();

}

void render()
{
    BeginDrawing();
    BeginMode2D(gCurrentScene->getState().camera);

    gShader.begin();

    gCurrentScene->render();
    sunglasses->render();
    gShader.setVector2("lightPosition", gLightPosition);
    gShader.setInt("isOn", shaderOn);
    for (Bullet* bullet : magazine) bullet->render();
    gShader.end();
    for (int x = 0; x <= SCREEN_WIDTH; x += 25) {
        DrawLine(x, 0, x, SCREEN_HEIGHT, Color{ 230, 41, 55, 100 });
    }
    for (int y = 0; y <= SCREEN_HEIGHT; y += 25) {
        DrawLine(0, y, SCREEN_WIDTH, y, Color{ 230, 41, 55, 100 });
    }
    
    gEffects->render();

    

    
    EndMode2D();
    DrawText(TextFormat("Player 1: %d", gCurrentScene->getState().playerOne->getLives()), 20, 20, 20, WHITE);
    DrawText(TextFormat("Player 2: %d", gCurrentScene->getState().playerTwo->getLives()), 800, 20, 20, WHITE);
    if (gCurrentScene->getState().playerOne->getLives() == 0)  DrawText("Player 2 Wins\n H for next", ORIGIN.x, ORIGIN.y, 100, WHITE);
    if (gCurrentScene->getState().playerTwo->getLives() == 0)  DrawText("Player 1 Wins\n H for next", ORIGIN.x - 300, ORIGIN.y, 100, WHITE);
    EndDrawing();
}

void shutdown() 
{
    for (Scene* scene : gLevels) {
        delete scene;
    }
    gShader.unload();

    CloseAudioDevice();
    CloseWindow();
}

void fireBullet(Entity *firingPlayer, Bullet *firingBullet){
    Direction startDirection = firingPlayer->getDirection();
    float finalAngle = firingPlayer->getAngle();
    
    if (startDirection == UP) finalAngle -= 90;
    if (startDirection == LEFT) finalAngle += 180;
    if (startDirection == DOWN) finalAngle += 90;
    firingBullet->activate();
    firingBullet->setPosition({40 * cos((finalAngle/360.0f) * 2.0f * 3.1415f) + firingPlayer->getPosition().x, 
        40 * sin((finalAngle/360.0f) * 2.0f * 3.1415f) + firingPlayer->getPosition().y});
    firingBullet->setAngle(finalAngle);
    firingBullet->setMovement({10.0f * (cos((finalAngle/360.0f) * 2.0f * 3.1415f)), 10.0f * (sin((finalAngle/360.0f) * 2.0f * 3.1415f))});
    std::cout << "firing at angle: " << finalAngle << " from: " << firingPlayer->getPosition().x << ", " << firingPlayer->getPosition().y << "\n";
    std::cout << "x: " << 10.0f * cos(finalAngle) << " y: " << 10.0f * sin(finalAngle);
    PlaySound(gCurrentScene->getState().shotSound);
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


