#pragma once

#include <string>
#include <vector>
#include <SDL3/SDL.h>

#include "scene_manager.h"
#include "utilities.h"
#include "paddle.h"
#include "ball.h"
#include "texture.h"

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
    static constexpr double BALL_START_ANGLE = PI / 3; // angle ball starts with in radians(randomly up or down)

    // game consts
    static constexpr int MAX_SCORE = 5;
    static constexpr int START_TIMER = 3; // in seconds
    static constexpr SDL_Color SCORE_COLOR = COLOR_WHITE;
    static constexpr SDL_Color VICTORY_COLOR = COLOR_YELLOW;

    explicit GameScene(SceneManager& scene_manager) 
                : mSceneManager(scene_manager),
                  mNumberTextures(10),
                  mLeftPaddle(0, 0, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_VELOCITY),
                  mRightPaddle(0, 0, PADDLE_WIDTH, PADDLE_HEIGHT, PADDLE_VELOCITY),
                  mBall(BALL_SPEED, BALL_SIZE)  
    {
    }

    void Init(App* appstate) override {
        mScreenWidth = appstate->screenW;
        mScreenHeight = appstate->screenH;

        // position paddles in starting positions
        mLeftPaddle.Reposition(mScreenWidth / 8, (mScreenHeight - PADDLE_HEIGHT) / 2 );
        mRightPaddle.Reposition(mScreenWidth * 7 / 8 - PADDLE_WIDTH, (mScreenHeight - PADDLE_HEIGHT) / 2);

        // initialize score and textures
        mLeftScore = 0;
        mRightScore = 0;
        for (int i = 0; i < mNumberTextures.size(); i++){
            std::string numText = std::to_string(i);
            
            if (!mNumberTextures[i].LoadText(appstate->renderer, numText.c_str(), appstate->fontMedium, SCORE_COLOR)){
                SDL_Log("Could not create number texture! Error: %s", SDL_GetError());
            }
        }

        // create textures for victory text
        std::string player1Text = "PLAYER-1";
        std::string player2Text = "PLAYER-2";
        std::string victoryText = "WINS";
        if (!mPlayer1Text.LoadText(appstate->renderer, player1Text.c_str(), appstate->fontLarge, COLOR_WHITE)){
            SDL_Log("Could not create player 1 victory texture! Error: %s", SDL_GetError());
        }
        
        if (!mPlayer2Text.LoadText(appstate->renderer, player2Text.c_str(), appstate->fontLarge, COLOR_WHITE)){
            SDL_Log("Could not create player 2 victory texture! Error: %s", SDL_GetError());
        }

        if (!mVictoryText.LoadText(appstate->renderer, victoryText.c_str(), appstate->fontLarge, VICTORY_COLOR)){
            SDL_Log("Could not create player 2 victory texture! Error: %s", SDL_GetError());
        }
        
        // init seperator line
        createDashedSeperator(mScreenWidth / 2, mScreenHeight, SEPERATOR_WIDTH, SEPERATOR_NUM, SEPERATOR_MARGIN);
        
        // init ball
        mBall.Init(appstate->mixer);
        resetBall();

        mIsGameIntro = true;
        mGameStartTime = SDL_GetTicks();
        mCurrCountdown = START_TIMER;
    }

    void Update(App *appstate, float delta) override {
        if (mIsGameOver){
            if (appstate->activeInputs[0]->InputStatePoll(appstate->player1InputState)){
                if (appstate->player1InputState.confirmPressed){
                    mRequestExit = true;
                }
            }
        }
        else if (mIsGameIntro){
            float gameStartDelta = (SDL_GetTicks() - mGameStartTime) / 1000.0f;
            mCurrCountdown = static_cast<int>(START_TIMER + 1 - gameStartDelta);
            if (mCurrCountdown == 0){
                mIsGameIntro = false;
            }
        }
        else{
            // poll player 1 input
            if (appstate->activeInputs[0]->InputStatePoll(appstate->player1InputState)){
                if (appstate->player1InputState.up){
                    mLeftPaddle.Move(Vector2D(0, -1), mScreenWidth, mScreenHeight, delta);
                }
                if (appstate->player1InputState.down){
                    mLeftPaddle.Move(Vector2D(0, 1), mScreenWidth, mScreenHeight, delta);
                }
            }

            // poll player2 input
            if (appstate->activeInputs[1]->InputStatePoll(appstate->player2InputState)){
                if (appstate->player2InputState.up){
                    mRightPaddle.Move(Vector2D(0, -1), mScreenWidth, mScreenHeight, delta);
                }
                if (appstate->player2InputState.down){
                    mRightPaddle.Move(Vector2D(0, 1), mScreenWidth, mScreenHeight, delta);
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

            checkGameOver(MAX_SCORE);
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

        if (mIsGameOver){
            renderGameOver(renderer);
        }

        if(mIsGameIntro){
            renderCountDown(renderer, mCurrCountdown, mScreenWidth / 2, mScreenHeight / 2);
        }

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

    std::vector<Texture> mNumberTextures;
    Texture mPlayer1Text;
    Texture mPlayer2Text;
    Texture mVictoryText;

    bool mIsGameIntro = false;
    bool mIsGameOver = false;
    bool mLeftVictory = false;

    Uint64 mGameStartTime = -1;
    int mCurrCountdown = -1;

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

    // current time should not exceed 9
    void renderCountDown(SDL_Renderer* renderer, int currentTime, int posX, int posY){
        if (currentTime > 9){
            return;
        }

        float tw, th;
        
        mNumberTextures[currentTime].GetSize(tw, th);
        SDL_FRect dest = {posX - tw / 2, posY - th / 2, tw, th};
        mNumberTextures[currentTime].Render(renderer, dest);
    }

    void renderScore(SDL_Renderer* renderer, int score, int posX, int posY, int margin){
        std::string digits = std::to_string(score);

        float totalWidth = 0, digitHeight = 0;
        for (char c : digits) {
            float tw, th;
            mNumberTextures[c - '0'].GetSize(tw, th);
            totalWidth += tw;
            digitHeight = th;
        }
        
        float currX = posX - totalWidth / 2;
        float currY = posY - digitHeight / 2;
        for (char c : digits) {
            float tw, th;
            mNumberTextures[c - '0'].GetSize(tw, th);
            SDL_FRect dest = {currX, currY, tw, th};
            mNumberTextures[c - '0'].Render(renderer, dest);
            currX += tw + margin;
        }
    }

    void resetBall(){
        mBall.Reposition((mScreenWidth - BALL_SIZE) / 2, (mScreenHeight - BALL_SIZE) / 2);
        int randDirection = SDL_rand(2); // 0 or 1 randomly
        int randAngle = SDL_rand(2);
        mBall.SetVelocity(Vector2D((randDirection * 2 - 1) * BALL_SPEED * std::sin(BALL_START_ANGLE), 
                                   (randAngle * 2 - 1) * BALL_SPEED * std::cos(BALL_START_ANGLE)));
        mBall.SetMaxVelocity(BALL_SPEED);
    }

    void checkGameOver(int maxScore){
        if (mLeftScore >= maxScore){
            mLeftVictory = true;
            mIsGameOver = true;
        }
        else if (mRightScore >= maxScore){
            mLeftVictory = false;
            mIsGameOver = true;
        }
    }

    void renderGameOver(SDL_Renderer* renderer){
        Texture &victorText = mLeftVictory ? mPlayer1Text : mPlayer2Text;

        float tw, th;
        victorText.GetSize(tw, th);
        SDL_FRect dest = { ((float)mScreenWidth - tw) / 2, (float)mScreenHeight / 4, tw, th };
        victorText.Render(renderer, dest);
        
        mVictoryText.GetSize(tw, th);
        dest = { ((float)mScreenWidth - tw) / 2, (float)mScreenHeight / 2, tw, th };
        mVictoryText.Render(renderer, dest);
    }
};