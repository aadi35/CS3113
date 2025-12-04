#include "Bullet.h"

Bullet::Bullet() : mPosition {0.0f, 0.0f}, mMovement {0.0f, 0.0f}, 
                   mVelocity {0.0f, 0.0f}, mAcceleration {0.0f, 0.0f},
                   mScale {DEFAULT_SIZE, DEFAULT_SIZE},
                   mColliderDimensions {DEFAULT_SIZE, DEFAULT_SIZE}, 
                   mTexture {NULL}, mTextureType {SINGLE}, mAngle {0.0f},
                   mSpriteSheetDimensions {}, mDirection {RIGHT}, 
                   mAnimationAtlas {{}}, mAnimationIndices {}, mFrameSpeed {0} { }

Bullet::Bullet(Vector2 position, Vector2 scale, const char *textureFilepath)
     : mPosition {position}, mVelocity {0.0f, 0.0f}, 
    mAcceleration {0.0f, 0.0f}, mScale {scale}, mMovement {0.0f, 0.0f}, 
    mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
    mTextureType {SINGLE}, mDirection {RIGHT}, mAnimationAtlas {{}}, 
    mAnimationIndices {}, mFrameSpeed {0}, mSpeed {DEFAULT_SPEED}, 
    mAngle {0.0f} { }

Bullet::Bullet(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, std::map<Direction, 
        std::vector<int>> animationAtlas) : 
        mPosition {position}, mVelocity {0.0f, 0.0f}, 
        mAcceleration {0.0f, 0.0f}, mMovement { 0.0f, 0.0f }, mScale {scale},
        mColliderDimensions {scale}, mTexture {LoadTexture(textureFilepath)}, 
        mTextureType {ATLAS}, mSpriteSheetDimensions {spriteSheetDimensions},
        mAnimationAtlas {animationAtlas}, mDirection {RIGHT},
        mAnimationIndices {animationAtlas.at(RIGHT)}, 
        mFrameSpeed {DEFAULT_FRAME_SPEED}, mAngle { 0.0f }, 
        mSpeed { DEFAULT_SPEED } { }

Bullet::~Bullet() { UnloadTexture(mTexture); };

void Bullet::checkCollisionY(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];
        
        if (isColliding(collidableEntity) && collidableEntity->isActive()){            
            if (collidableEntity->getEntityType() == BLOCK){
                    collidableEntity->deactivate();
                    deactivate();}
            if (collidableEntity->getEntityType() == PLAYER){
                collidableEntity->loseLife();
                deactivate();
            }
        }
    }
}

void Bullet::checkCollisionX(Entity *collidableEntities, int collisionCheckCount)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];
        
        if (isColliding(collidableEntity) && collidableEntity->isActive()){            
            if (collidableEntity->getEntityType() == BLOCK){
                    collidableEntity->deactivate();
                    deactivate();}
            if (collidableEntity->getEntityType() == PLAYER){
                collidableEntity->loseLife();
                deactivate();
            }
        }
    }
}

void Bullet::checkCollisionY(Map *map)
{
    if (map == nullptr) return;

    Vector2 topCentreProbe    = { mPosition.x, mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topLeftProbe      = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };
    Vector2 topRightProbe     = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y - (mColliderDimensions.y / 2.0f) };

    Vector2 bottomCentreProbe = { mPosition.x, mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomLeftProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };
    Vector2 bottomRightProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y + (mColliderDimensions.y / 2.0f) };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ABOVE (jumping upward)
    if ((map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap)   ||
         map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap)) && mVelocity.y < 0.0f)
    {
        mPosition.y += yOverlap;   // push down
        mMovement.y  *= -1;
        mCollisionCount += 1;
        if (mCollisionCount == 10){ 
            deactivate();
            mCollisionCount = 0;
        }
        mIsCollidingTop = true;
    }

    // COLLISION BELOW (falling downward)
    if ((map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap)   ||
         map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap)) && mVelocity.y > 0.0f)
    {
        mPosition.y -= yOverlap;   // push up
        mMovement.y  *= -1;
        mCollisionCount += 1;
        if (mCollisionCount == 10){ 
            deactivate();
            mCollisionCount = 0;
        }
        mIsCollidingBottom = true;
    } 
}

void Bullet::checkCollisionX(Map *map)
{
    if (map == nullptr) return;

    Vector2 leftCentreProbe   = { mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y };

    Vector2 rightCentreProbe  = { mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ON RIGHT (moving right)
    if (map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap) 
         && mVelocity.x > 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x -= xOverlap * 1.01f;   // push left
        mMovement.x  *= -1;
        mCollisionCount += 1;
        if (mCollisionCount == 2){ 
            deactivate();
            mCollisionCount = 0;
        }
        mIsCollidingRight = true;
    }

    // COLLISION ON LEFT (moving left)
    if (map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap) 
         && mVelocity.x < 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x += xOverlap * 1.01;   // push right
        mMovement.x  *= -1;
        mCollisionCount += 1;
        if (mCollisionCount == 2){ 
            deactivate();
            mCollisionCount = 0;
        }
        mIsCollidingLeft = true;
    }
}

bool Bullet::isColliding(Entity *collidableEntity) const 
{
    float xDistance = fabs(mPosition.x - collidableEntity->getPosition().x) - 
        ((mColliderDimensions.x + collidableEntity->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(mPosition.y - collidableEntity->getPosition().y) - 
        ((mColliderDimensions.y + collidableEntity->getColliderDimensions().y) / 2.0f);

    if (xDistance < 0.0f && yDistance < 0.0f) return true;

    return false;
}

void Bullet::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;
    float framesPerSecond = 1.0f / mFrameSpeed;

    if (mAnimationTime >= framesPerSecond)
    {
        mAnimationTime = 0.0f;

        mCurrentFrameIndex++;
        mCurrentFrameIndex %= mAnimationIndices.size();
    }
}

void Bullet::AIWander() { moveLeft(); }

void Bullet::AIFollow(Entity *target)
{
    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, target->getPosition()) < 250.0f) 
            mAIState = WALKING;
        break;

    case WALKING:
        // Depending on where the player is in respect to their x-position
        // Change direction of the enemy
        if (mPosition.x > target->getPosition().x) moveLeft();
        else                                       moveRight();
    
    default:
        break;
    }
}

void Bullet::AIActivate(Entity *target)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;

    case FOLLOWER:
        AIFollow(target);
        break;
    
    default:
        break;
    }
}

void Bullet::update(float deltaTime, Entity *playerOne, Entity *playerTwo, Map *map, 
    Entity *collidableEntities, int collisionCheckCount)
{
    if (mBulletStatus == BINACTIVE) return;

    resetColliderFlags();

    normaliseMovement();

    mVelocity.x = mMovement.x * mSpeed;
    mVelocity.y = mMovement.y * mSpeed;

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    // ––––– JUMPING ––––– //
    if (mIsJumping)
    {
        // STEP 1: Immediately return the flag to its original false state
        mIsJumping = false;
        
        // STEP 2: The player now acquires an upward velocity
        mVelocity.y -= mJumpingPower;
    }

    

    mPosition.y += mVelocity.y * deltaTime;
    checkCollisionY(collidableEntities, collisionCheckCount);
    checkCollisionY(playerOne, 1);
    checkCollisionY(playerTwo, 1);
    checkCollisionY(map);

    mPosition.x += mVelocity.x * deltaTime;
    checkCollisionX(collidableEntities, collisionCheckCount);
    checkCollisionX(playerOne, 1);
    checkCollisionX(playerTwo, 1);
    checkCollisionX(map);

    

    if (mTextureType == ATLAS && GetLength(mMovement) != 0 && mIsCollidingBottom) 
        animate(deltaTime);
}

void Bullet::render()
{
    if(mBulletStatus == BINACTIVE) return;

    Rectangle textureArea;

    switch (mTextureType)
    {
        case SINGLE:
            // Whole texture (UV coordinates)
            textureArea = {
                // top-left corner
                0.0f, 0.0f,

                // bottom-right corner (of texture)
                static_cast<float>(mTexture.width),
                static_cast<float>(mTexture.height)
            };
            break;
        case ATLAS:
            textureArea = getUVRectangle(
                &mTexture, 
                mAnimationIndices[mCurrentFrameIndex], 
                mSpriteSheetDimensions.x, 
                mSpriteSheetDimensions.y
            );
        
        default: break;
    }

    // Destination rectangle – centred on gPosition
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    // Render the texture on screen
    DrawTexturePro(
        mTexture, 
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );

    // displayCollider();
}

void Bullet::displayCollider() 
{
    // draw the collision box
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,  
        mPosition.y - mColliderDimensions.y / 2.0f,  
        mColliderDimensions.x,                        
        mColliderDimensions.y                        
    };

    DrawRectangleLines(
        colliderBox.x,      // Top-left X
        colliderBox.y,      // Top-left Y
        colliderBox.width,  // Width
        colliderBox.height, // Height
        GREEN               // Color
    );
}