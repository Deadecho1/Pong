#pragma once

#include <string>
#include <vector>
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "scene_manager.h"
#include "utilities.h"
#include "paddle.h"
#include "ball.h"

class GameScene: public Scene
{
public:
    // paddle consts
    static constexpr int PADDLE_WIDTH = 10; // in px
    static constexpr int PADDLE_HEIGHT = 30; // in px
    static constexpr int PADDLE_VELOCITY = 450; // in px per second
    // dashed line seperator consts
    static constexpr int SEPERATOR_NUM = 20;
    static constexpr int SEPERATOR_WIDTH = 6;
    static constexpr int SEPERATOR_MARGIN = 8;
    // score consts
    static constexpr float LEFT_SCORE_POS_X = 0.3f; // in percent relative to screen width
    static constexpr float RIGHT_SCORE_POS_X = 0.7f;
    static constexpr float SCORE_POS_Y = 1.0f / 12.0f;

    // ball consts
    static constexpr float BALL_SPEED = 300.0f;
    static constexpr int BALL_SIZE = 10;

    static constexpr SDL_Color SCORE_COLOR = COLOR_WHITE;

    explicit GameScene(SceneManager& scene_manager) 
                : mSceneManager(scene_manager),
                  mNumberTextures(10, nullptr),
                  mLeftPaddle(0, 0, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_VELOCITY),
                  mRightPaddle(0, 0, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_VELOCITY),
                  mBall(BALL_SPEED, BALL_SIZE)  
    {
    }

    void Init(App* appstate) override {
        mScreenWidth = appstate->screenW;
        mScreenHeight = appstate->screenH;

        // position paddles in starting positions
        mLeftPaddle.Reposition(appstate->screenW / 8, (appstate->screenH - PADDLE_HEIGHT) / 2 );
        mRightPaddle.Reposition(appstate->screenW * 7 / 8 - PADDLE_WIDTH, (appstate->screenH - PADDLE_HEIGHT) / 2);

        // initialize score and textures
        mLeftScore = 0;
        mRightScore = 0;
        for (int i = 0; i < mNumberTextures.size(); i++){
            std::string numText = std::to_string(i);
            SDL_Surface* tempSurface = TTF_RenderText_Blended(appstate->fontMedium, numText.c_str(), 0, SCORE_COLOR);
            SDL_Texture* numTexture = SDL_CreateTextureFromSurface(appstate->renderer, tempSurface);
            if (!numTexture){
                SDL_Log("Could not create number texture! Error: %s", SDL_GetError());
            }
            mNumberTextures[i] = numTexture;

            SDL_DestroySurface(tempSurface);
        }

        // init seperator line
        createDashedSeperator(appstate->screenW / 2, appstate->screenH, SEPERATOR_WIDTH, SEPERATOR_NUM, SEPERATOR_MARGIN);
        
        resetBall();
    }

    void Update(App *appstate, float delta) override {
        // poll player 1 input
        if (appstate->activeInputs[0]->InputStatePoll(appstate->player1InputState)){
            if (appstate->player1InputState.up){
                mLeftPaddle.Move(Vector2D(0, -1), appstate->screenW, appstate->screenH, delta);
            }
            if (appstate->player1InputState.down){
                mLeftPaddle.Move(Vector2D(0, 1), appstate->screenW, appstate->screenH, delta);
            }
        }

        // poll player2 input
        if (appstate->activeInputs[1]->InputStatePoll(appstate->player2InputState)){
            if (appstate->player2InputState.up){
                mRightPaddle.Move(Vector2D(0, -1), appstate->screenW, appstate->screenH, delta);
            }
            if (appstate->player2InputState.down){
                mRightPaddle.Move(Vector2D(0, 1), appstate->screenW, appstate->screenH, delta);
            }
        }

        mBall.Update(delta, {&mLeftPaddle, &mRightPaddle}, mScreenHeight);

        int ballX, ballY;
        mBall.GetPosition(ballX, ballY);
        // left score
        if (ballX > mScreenWidth){
            mLeftScore += 1;
            resetBall();
        }
        // right score
        else if (ballX < 0){
            mRightScore += 1;
            resetBall();
        }
    }

    void Render(SDL_Renderer* renderer) override {
        // render black background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // draw paddles
        mLeftPaddle.Render(renderer);
        mRightPaddle.Render(renderer);

        // draw dashed line seperator
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRects(renderer, mSeperatorRects.data(), mSeperatorRects.size());

        // draw score
        renderScore(renderer, mLeftScore, mScreenWidth * LEFT_SCORE_POS_X, mScreenHeight * SCORE_POS_Y, 4);
        renderScore(renderer, mRightScore, mScreenWidth * RIGHT_SCORE_POS_X, mScreenHeight * SCORE_POS_Y, 4);

        mBall.Render(renderer);

        SDL_RenderPresent(renderer);
    }

    void OnEvent(App* appstate, SDL_Event *event) override {
        
    }

private:
    SceneManager& mSceneManager;

    int mScreenWidth;
    int mScreenHeight;

    Paddle mLeftPaddle;
    Paddle mRightPaddle;
    std::vector<SDL_FRect> mSeperatorRects;

    Ball mBall;

    unsigned int mLeftScore;
    unsigned int mRightScore;

    std::vector<SDL_Texture*> mNumberTextures;

    void createDashedSeperator(int xPos, int lineHeight, int width, int density, int margin){
        float seperatorHeight = (lineHeight / density) - (margin * 2);
        for (int y = 0; y < lineHeight; y += seperatorHeight + margin * 2){
            mSeperatorRects.push_back(
                {
                    static_cast<float>(xPos - width / 2),
                    static_cast<float>(y + margin),
                    static_cast<float>(width), 
                    seperatorHeight
                }
            );
        }
    }

    void renderScore(SDL_Renderer* renderer, int score, int posX, int posY, int margin){
        std::string digits = std::to_string(score);

        float totalWidth = 0, digitHeight = 0;
        for (char c : digits) {
            float tw, th;
            SDL_GetTextureSize(mNumberTextures[c - '0'], &tw, &th);
            totalWidth += tw;
            digitHeight = th;
        }
        
        float currX = posX - totalWidth / 2;
        float currY = posY - digitHeight / 2;
        for (char c : digits) {
            float tw, th;
            SDL_GetTextureSize(mNumberTextures[c - '0'], &tw, &th);
            SDL_FRect dest = {currX, currY, tw, th};
            SDL_RenderTexture(renderer, mNumberTextures[c - '0'], NULL, &dest);
            currX += tw + margin;
        }
    }

    void resetBall(){
        mBall.Reposition((mScreenWidth - BALL_SIZE) / 2, (mScreenHeight - BALL_SIZE) / 2);
        int rand = SDL_rand(2); // 0 or 1 randomly
        mBall.SetVelocity(Vector2D((rand * 2 - 1) * BALL_SPEED, 0));
        mBall.SetMaxVelocity(BALL_SPEED);
    }
};