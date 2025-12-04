#include "LevelB.h"

LevelB::LevelB()                                      : Scene { {0.0f}, nullptr   } {}
LevelB::LevelB(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelB::~LevelB() { shutdown(); }

Entity *enemy =nullptr;


void LevelB::initialise()
{
   mGameState.nextSceneID = 0;

   mGameState.bgm = LoadMusicStream("assets/game/bgm.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.shotSound = LoadSound("assets/game/Shot.wav");

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/game/tileset.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      4, 1,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PLAYER1 -----------
   */
   std::map<Direction, std::vector<int>> playerOneAnimationAtlas = {
      {DOWN,  { 6, 7 }},
      {LEFT,  { 4, 5 }},
      {UP,    { 2, 3 }},
      {RIGHT, { 0, 1 }},
   };

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.playerOne = new Entity(
      {mOrigin.x - 190, mOrigin.y - 10}, // position
      {TILE_DIMENSION, TILE_DIMENSION},             // scale
      "assets/game/Player1.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 2, 4 },                                 // atlas dimensions
      playerOneAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.playerOne->setColliderDimensions({
      mGameState.playerOne->getScale().x / 3.5f,
      mGameState.playerOne->getScale().y / 3.0f
   });
   mGameState.playerOne->setAcceleration({0.0f, 0.0f});

   /*
      ----------- PLAYER2 -----------
   */
   std::map<Direction, std::vector<int>> playerTwoAnimationAtlas = {
      {DOWN,  { 6, 7 }},
      {LEFT,  { 4, 5 }},
      {UP,    { 2, 3 }},
      {RIGHT, { 0, 1 }},
   };

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.playerTwo = new Entity(
      {mOrigin.x + 190, mOrigin.y - 10}, // position
      {TILE_DIMENSION, TILE_DIMENSION},             // scale
      "assets/game/Player2.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 2, 4 },                                 // atlas dimensions
      playerTwoAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.playerTwo->setColliderDimensions({
      mGameState.playerTwo->getScale().x / 3.5f,
      mGameState.playerTwo->getScale().y / 3.0f
   });
   mGameState.playerTwo->setAcceleration({0.0f, 0.0f});

   /*
      ----------- CAMERA -----------
   */
   mGameState.camera = { 0 };                                    // zero initialize
   mGameState.camera.target = mOrigin; // camera follows player
   mGameState.camera.offset = mOrigin;                           // camera offset to center of screen
   mGameState.camera.rotation = 0.0f;                            // no rotation
   mGameState.camera.zoom = 1.0f;                                // default zoom

   /*
   ----------- BOXES -----------
   */
   std::vector<int> used;
   mGameState.boxes = new Entity[40];
   for (int i = 0; i < 40; i++){
         int rand = GetRandomValue(0, 836);
         while (std::find(used.begin(), used.end(), rand) != used.end()){
            int rand = GetRandomValue(0, 836);
         }
         mGameState.boxes[i].setPosition({((rand % 38) + 1.5f) * 25.0f ,((rand % 22) + 1.5f) * 25.0f });
         mGameState.boxes[i].setScale({25.0f, 25.0f});
         mGameState.boxes[i].setTexture("assets/game/Box.png");
         mGameState.boxes[i].setEntityType(BLOCK);
   }

   /*
      ----------- NPC -----------
   */
   enemy = new Entity({mOrigin.x - 10, mOrigin.y - 10},{TILE_DIMENSION, TILE_DIMENSION}, "assets/game/Bomb.png", NPC);
   enemy->setAIType(WANDERER);
   
}

void LevelB::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.playerOne->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      mGameState.boxes,        // collidable entities
      20               // col. entity count
   );
   mGameState.playerTwo->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      mGameState.boxes,        // collidable entities
      20               // col. entity count
   );
   
   enemy->update(
      deltaTime,
      mGameState.playerOne,
      mGameState.map,
      mGameState.playerTwo
   );
   enemy->resetMovement();

   Vector2 currentPlayerPosition = { mGameState.playerOne->getPosition().x, mOrigin.y };

   for (int i = 0; i < 40; i++) mGameState.boxes[i].update(deltaTime, nullptr, mGameState.map, nullptr, 0);

   if (mGameState.playerOne->getPosition().y > 800.0f) mGameState.nextSceneID = 1;

   //panCamera(&mGameState.camera, &currentPlayerPosition);
}

void LevelB::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.playerOne->render();
   mGameState.playerTwo->render();
   mGameState.map->render();
   enemy->render();
   for (int i = 0; i < 40; i++) mGameState.boxes[i].render();
}

void LevelB::shutdown()
{
   delete mGameState.playerOne;
   delete mGameState.playerTwo;
   delete mGameState.map;
   //delete [] mGameState.boxes;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.shotSound);
}