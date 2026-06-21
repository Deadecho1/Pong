#pragma once

#include <functional>
#include <string>
#include <SDL3/SDL.h>

#include "scene.h"
#include "scene_manager.h"
#include "utilities.h"
#include "game_scene.h"
#include "texture.h"

static const std::string TITLE_TEXT = "PONG";
static const std::string OPTION1_TEXT = "PLAY";
static const std::string OPTION2_TEXT = "EXIT";

static constexpr int TITLE_PADDING = 100;
static constexpr int OPTIONS_PADDING = 30;

static constexpr float COLOR_SPEED = 10.0f;
static constexpr SDL_Color OPTION_COLOR = COLOR_WHITE;
static constexpr SDL_Color OPTION_COLOR_SELECTED = COLOR_YELLOW;

class MainMenuScene : public Scene {
public:
    explicit MainMenuScene(SceneManager& scene_manager) : mSceneManager(scene_manager) {}

    void Init(App* appstate) override {
        // create title
        if (!mTitleTexture.LoadText(appstate->renderer, TITLE_TEXT, appstate->fontLarge, COLOR_WHITE)){
            SDL_Log("Could not create title texture! Error: %s", SDL_GetError());
        }

        // create menu options
        auto playOnClick = [this](App* app)
        {
            mSceneManager.PushScene(
                std::make_unique<GameScene>(mSceneManager),
                app
            );
        };
        auto exitOnClick = [](App* app) { app->shouldExit = true; };
        createMenuOption(appstate->renderer, appstate->fontMedium, OPTION1_TEXT, playOnClick);
        createMenuOption(appstate->renderer, appstate->fontMedium, OPTION2_TEXT, exitOnClick);

        mScreenW = appstate->screenW;
        mScreenH = appstate->screenH;
    }

    void OnEvent(App* appstate, SDL_Event *event) override {}

    void Update(App* appstate, float delta) override {
        // poll player 1 input
        if (appstate->activeInputs[0]->InputStatePoll(appstate->player1InputState)){
            if (appstate->player1InputState.upPressed){
                SDL_Log("[PLAYER1] Up!");
                mSelectedIndex = (mSelectedIndex - 1 + mMenuOptions.size()) % mMenuOptions.size();
                mColorTimer = 0.0f;
            }
            if (appstate->player1InputState.downPressed){
                SDL_Log("[PLAYER1] Down!");
                mSelectedIndex = (mSelectedIndex + 1) % mMenuOptions.size();
                mColorTimer = 0.0f;
            }
            if (appstate->player1InputState.confirmPressed){
                mMenuOptions[mSelectedIndex]->callback(appstate);
            }
            if (appstate->player1InputState.escPressed){
                SDL_Log("[PLAYER1] Escape!");
            }
        }

        // poll player2 input
        if (appstate->activeInputs[1]->InputStatePoll(appstate->player2InputState)){
            if (appstate->player2InputState.upPressed){
                SDL_Log("[PLAYER2] Up!");
                mSelectedIndex = (mSelectedIndex - 1 + mMenuOptions.size()) % mMenuOptions.size();
                mColorTimer = 0.0f;
            }
            if (appstate->player2InputState.downPressed){
                SDL_Log("[PLAYER2] Down!");
                mSelectedIndex = (mSelectedIndex + 1) % mMenuOptions.size();
                mColorTimer = 0.0f;
            }
        }

        mColorTimer += delta * COLOR_SPEED;
        mSelectionColor = LerpColor(OPTION_COLOR, OPTION_COLOR_SELECTED, (SDL_sinf(mColorTimer) + 1) / 2);
    }

    void Render(SDL_Renderer* renderer) override {
        // render black background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        
        // render title
        float tw, th;
        mTitleTexture.GetSize(tw, th);
        float midX = (mScreenW - tw) / 2;
        SDL_FRect dest = {midX, TITLE_PADDING, tw, th};
        mTitleTexture.Render(renderer, dest);

        // render menu options
        for (int i = 0; i < mMenuOptions.size(); i++){
            mMenuOptions[i]->GetSize(tw, th);
            float midX = (mScreenW - tw) / 2;
            float midY = (mScreenH - th) / 2;
            SDL_FRect dest = {midX, OPTIONS_PADDING + midY + i * (th + OPTIONS_PADDING), tw, th};

            // play animation on selected item
            if (mSelectedIndex == i){
                mMenuOptions[i]->texture.SetColorMod(mSelectionColor.r, mSelectionColor.g, mSelectionColor.b);
            }
            else{
                mMenuOptions[i]->texture.SetColorMod(OPTION_COLOR.r, OPTION_COLOR.g, OPTION_COLOR.b);
            }

            mMenuOptions[i]->texture.Render(renderer, dest);
        }

        SDL_RenderPresent(renderer);
    }

private:

    struct MenuOption 
    {
        Texture texture;
        std::function<void(App*)> callback;

        void GetSize(float& w, float& h){ texture.GetSize(w, h); }
    };

    SceneManager& mSceneManager;

    Texture mTitleTexture;

    int mSelectedIndex = 0;
    std::vector<std::unique_ptr<MenuOption>> mMenuOptions;

    int mScreenW;
    int mScreenH;

    float mColorTimer = 0.0f;
    SDL_Color mSelectionColor = OPTION_COLOR;

    void createMenuOption(SDL_Renderer* renderer, TTF_Font* font, std::string text, std::function<void(App*)> callback = nullptr)
    {
        std::unique_ptr<MenuOption> option = std::make_unique<MenuOption>();
        if(!option->texture.LoadText(renderer, text, font, OPTION_COLOR)){
            SDL_Log("Could not create option %s texture! Error: %s", text, SDL_GetError());
        }

        option->callback = callback;

        mMenuOptions.push_back(std::move(option));
    }
};