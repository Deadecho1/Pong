#pragma once

#include <SDL3/SDL.h>

#include "utilities.h"

class Paddle{
public:
    Paddle(int initPosX, int initPosY, int width, int height, int maxVelocity): 
        mPosX(initPosX), mPosY(initPosY), mCollisionBox({ initPosX, initPosY, width, height}), 
        mVel(0.0, 0.0), mMaxVelocity(maxVelocity) {}

    // dir should be a normalized vector
    void Move(const Vector2D& dir, int boundsX, int boundsY, float deltaTime)
    {
        float origPosY = mPosY;

        float xMovement = dir.x * mMaxVelocity * deltaTime;
        mPosX += xMovement;
        if (mPosX > boundsX - mCollisionBox.w || mPosX < 0){
            mPosX -= xMovement;
        }

        float yMovement = dir.y * mMaxVelocity * deltaTime;
        mPosY += yMovement;
        if (mPosY > boundsY - mCollisionBox.h || mPosY < 0){
            mPosY -= yMovement;
        }

        mCollisionBox.x = static_cast<int>(mPosX);
        mCollisionBox.y = static_cast<int>(mPosY);

        if (origPosY < mPosY){
            mVel.y = mMaxVelocity;
        }
        else if (origPosY > mPosY){
            mVel.y = -mMaxVelocity;
        }
        else{
            mVel.y = 0;
        }
    }

    void Reposition(int x, int y)
    {
        mPosX = x;
        mPosY = y;
        mCollisionBox.x = x;
        mCollisionBox.y = y;
    }

    void Render(SDL_Renderer* renderer){
        SDL_FRect renderRect = {
            mPosX,
            mPosY,
            static_cast<float>(mCollisionBox.w),
            static_cast<float>(mCollisionBox.h)
        };

        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255 ,255);
        SDL_RenderFillRect(renderer, &renderRect);
    }

    const SDL_Rect& GetRect() { return mCollisionBox; }

    const Vector2D& GetVelocity() { return mVel; }

private:
    SDL_Rect mCollisionBox; // will also be used for rendering
    float mPosX, mPosY; // top left of rectangle
    Vector2D mVel;
    float mMaxVelocity;

};