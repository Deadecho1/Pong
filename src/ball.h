#pragma once

#include <vector>
#include <cmath>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "utilities.h"
#include "paddle.h"
#include "audio_clip.h"

constexpr float MAX_ANGLE = 75.0f * PI / 180.0f;
constexpr float SPEED_INCREASE = 20.0f;

class Ball{
public:
    Ball(float speed, int size)
         : mMaxVelocity(speed), 
           mVelocity(0.0f, 0.0f),
           mSize(size),
           mPosX(0),
           mPosY(0)
    {
    }
    
    void Init(MIX_Mixer* mixer){
        mBounceSound.LoadClip(mixer, (AUDIO_PATH) + "bounce.mp3");
    }
        
    void Reposition(int x, int y){
        mPosX = x;
        mPosY = y;
    }

    void SetVelocity(const Vector2D &vel){
        mVelocity = vel;
    }

    void SetMaxVelocity(float vel){
        mMaxVelocity = vel;
    }

    void GetPosition(int &posX, int &posY){ 
        posX = mPosX; posY = mPosY; 
    }

    void Update(float delta, std::vector<Paddle*> paddles, int boundY){
        float midY = mPosY + mSize / 2;
        float origVelX = mVelocity.x;
        float origVelY = mVelocity.y;

        mPosX += origVelX * delta;
        float ballMid = mPosX + mSize / 2.0f;
        for (Paddle* paddle : paddles) {
            float paddleMid = paddle->GetRect().x + paddle->GetRect().w / 2.0f;
            // check if ball is heading towards paddle, skip collisions otherwise
            if (!((ballMid < paddleMid) == (mVelocity.x > 0))){
                continue;
            }
            if (checkCollision(paddle->GetRect()) == true){
                const SDL_Rect& rect = paddle->GetRect();
                float ballMidY   = mPosY + mSize / 2.0f;
                float paddleMidY = rect.y + rect.h / 2.0f;

                // -1 = top of paddle, +1 = bottom of paddle
                float relativeHit = (ballMidY - paddleMidY) / (rect.h / 2.0f);
                relativeHit = std::max(-1.0f, std::min(1.0f, relativeHit));

                float bounceAngle = relativeHit * MAX_ANGLE;

                // increase velocity on hit
                mMaxVelocity += SPEED_INCREASE;

                float xDir = (mVelocity.x > 0) ? -1.0f : 1.0f;
                mVelocity.x = xDir * mMaxVelocity * std::cos(bounceAngle);
                mVelocity.y = mMaxVelocity * std::sin(bounceAngle);

                mPosX -= origVelX * delta;

                mBounceSound.Play();
            }
        }

        mPosY += origVelY * delta;
        if (mPosY < 0 || mPosY + mSize > boundY){
            mVelocity.y *= -1; // change direction on y axis;
            mPosY -= origVelY * delta;

            mBounceSound.Play();
        }
    }

    void Render(SDL_Renderer* renderer){
        // white ball
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

        SDL_FRect dest = { static_cast<float>(mPosX), 
                            static_cast<float>(mPosY),
                            static_cast<float>(mSize), 
                            static_cast<float>(mSize) };
        SDL_RenderFillRect(renderer, &dest);
    }

private:
    float mPosX;
    float mPosY;
    Vector2D mVelocity;
    float mMaxVelocity;
    int mSize;
    AudioClip mBounceSound;

    bool checkCollision(const SDL_Rect &object){
        int leftA, leftB;
        int rightA, rightB;
        int topA, topB;
        int bottomA, bottomB;

        leftA = mPosX;
        rightA = mPosX + mSize;
        topA = mPosY;
        bottomA = mPosY + mSize;

        leftB = object.x;
        rightB = object.x + object.w;
        topB = object.y;
        bottomB = object.y + object.h;

        // if any of the sides from A are outside B
        if (bottomA <= topB){
            return false;
        }

        if (rightA <= leftB){
            return false;
        }

        if (leftA >= rightB){
            return false;
        }

        if (topA >= bottomB){
            return false;
        }

        // if none of the sides from A are outside B 
        return true;
    }
};