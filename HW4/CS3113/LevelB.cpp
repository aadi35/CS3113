#include "LevelB.h"

LevelB::LevelB()                                      : Scene { {0.0f}, nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

void LevelB::initialise()
{
   mGameState.nextSceneID = 0;

   mGameState.bgm = LoadMusicStream("assets/game/time_for_adventure.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/game/jump.wav");

   std::map<Direction, std::vector<int>> slimeAnimationAtlas = {
         {DOWN,  {  0,  1,  2,  3,  4,  5,  6,  7, 8}},
         {LEFT,  {  0,  1,  2,  3,  4,  5,  6,  7, 8}},
         {UP,    {  0,  1,  2,  3,  4,  5,  6,  7, 8}},
         {RIGHT, {  0,  1,  2,  3,  4,  5,  6,  7, 8}},
      };

   mGameState.enemy = new Entity({mOrigin.x + 600.0f, mOrigin.y - 300.0f}, {50.0f, 50.0f}, "assets/game/slime_green.png", ATLAS, {3, 4}, slimeAnimationAtlas, NPC);
   mGameState.enemy->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
   mGameState.enemy->setAIType(FOLLOWER);
   mGameState.enemy->setAIState(IDLE);
   mGameState.enemy->setSpeed(Entity::DEFAULT_SPEED * 0.5);
   mGameState.enemy->setJumpingPower(200.0f);




   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/game/platforms.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      4, 4,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> protagAnimationAtlas = {
      {DOWN,  {  0,  1,  2,  3,  4,  5,  6,  7 }},
      {LEFT,  {  8,  9, 10, 11, 12, 13, 14, 15 }},
      {UP,    { 24, 25, 26, 27, 28, 29, 30, 31 }},
      {RIGHT, { 40, 41, 42, 43, 44, 45, 46, 47 }},
   };

   float sizeRatio  = 48.0f / 64.0f;

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.protag = new Entity(
      {mOrigin.x - 600.0f, mOrigin.y - 300.0f}, // position
      {200.0f * sizeRatio, 200.0f},             // scale
      "assets/game/walk.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 6, 8 },                                 // atlas dimensions
      protagAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.protag->setJumpingPower(550.0f);
   mGameState.protag->setColliderDimensions({
      mGameState.protag->getScale().x / 3.5f,
      mGameState.protag->getScale().y / 3.0f
   });
   mGameState.protag->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mGameState.protag->getPosition(); // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom
}

void LevelB::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);


   if (mGameState.enemy->isCollidingBottom()) mGameState.enemy->jump();

   mGameState.enemy->update(deltaTime, mGameState.protag, mGameState.map, nullptr, 0);

   mGameState.protag->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      nullptr,        // collidable entities
      0               // col. entity count
   );

   // CAMERA
   Vector2 currentPlayerPosition = { mGameState.protag->getPosition().x, mOrigin.y };

   if (mGameState.protag->getPosition().x > mOrigin.x + 600.0f) {
      mGameState.nextSceneID = 3;
   }

   panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelB::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.protag->render();
   mGameState.enemy->render();
   mGameState.map->render();
   
}

void LevelB::shutdown()
{
   delete mGameState.protag;
   delete mGameState.map;
   delete mGameState.enemy;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
}