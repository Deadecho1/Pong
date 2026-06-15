#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include <functional>
#include "scene.h"
#include "scene_manager.h"
#include "utilities.h"
#include "game_scene.h"

static constexpr float COLOR_SPEED = 10.0f;
static constexpr SDL_Color OPTION_COLOR = COLOR_WHITE;
static constexpr SDL_Color OPTION_COLOR_SELECTED = COLOR_YELLOW;

class MainMenuScene : public Scene {
public:
    explicit MainMenuScene(SceneManager& scene_manager) : mSceneManager(scene_manager) {}

    void Init(App* appstate) override {
        SDL_Surface* tempSurface;
        // create title
        tempSurface = TTF_RenderText_Blended(appstate->fontLarge, "PONG", 0, COLOR_WHITE);
        mTitleTexture = SDL_CreateTextureFromSurface(appstate->renderer, tempSurface);
        if (!mTitleTexture){
            SDL_Log("Could not create title texture! Error: %s", SDL_GetError());
        }
        SDL_DestroySurface(tempSurface);

        // create menu options
        auto playOnClick = [this](App* app)
        {
            mSceneManager.PushScene(
                std::make_unique<GameScene>(mSceneManager),
                app
            );
        };
        auto exitOnClick = [](App* app) { app->shouldExit = true; };
        createMenuOption(appstate->renderer, appstate->fontMedium, "PLAY", playOnClick);
        createMenuOption(appstate->renderer, appstate->fontMedium, "EXIT", exitOnClick);

        mScreenW = appstate->screenW;
        mScreenH = appstate->screenH;
    }

    void OnEvent(App* appstate, SDL_Event *event) override {

    }

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

        
        float midX, midY;
        float tw, th;
        // render title
        float titlePadding = 100;
        SDL_GetTextureSize(mTitleTexture, &tw, &th);
        midX = (mScreenW - tw) / 2;
        SDL_FRect dest = {midX, titlePadding, tw, th};
        SDL_RenderTexture(renderer, mTitleTexture, NULL, &dest);

        // render menu options
        int optionsPadding = 30; // padding in px
        for (int i = 0; i < mMenuOptions.size(); i++){
            mMenuOptions[i]->GetSize(tw, th);
            midX = (mScreenW - tw) / 2;
            midY = (mScreenH - th) / 2;
            SDL_FRect dest = {midX, midY + i * (th + optionsPadding), tw, th};
            // play animation on selected item
            if (mSelectedIndex == i){
                SDL_SetTextureColorMod(mMenuOptions[i]->texture, mSelectionColor.r, mSelectionColor.g, mSelectionColor.b);
            }
            else{
                SDL_SetTextureColorMod(mMenuOptions[i]->texture, OPTION_COLOR.r, OPTION_COLOR.g, OPTION_COLOR.b);
            }
            SDL_RenderTexture(renderer, mMenuOptions[i]->texture, NULL, &dest);
        }

        SDL_RenderPresent(renderer);
    }

    ~MainMenuScene()
    {
        SDL_DestroyTexture(mTitleTexture);
        for (int i = 0; i < mMenuOptions.size(); i++){
            SDL_DestroyTexture(mMenuOptions[i]->texture);
        }
    }
private:

    struct MenuOption 
    {
        SDL_Texture* texture;
        std::function<void(App*)> callback;

        void GetSize(float& w, float& h){ if(texture != nullptr) SDL_GetTextureSize(texture, &w, &h); }
    };

    SceneManager& mSceneManager;

    SDL_Texture* mTitleTexture;

    int mSelectedIndex = 0;
    std::vector<std::unique_ptr<MenuOption>> mMenuOptions;

    int mScreenW;
    int mScreenH;

    float mColorTimer = 0.0f;
    SDL_Color mSelectionColor = OPTION_COLOR;

    void createMenuOption(SDL_Renderer* renderer, TTF_Font* font, const char* text, std::function<void(App*)> callback = nullptr)
    {
        SDL_Surface* tempSurface;
        tempSurface = TTF_RenderText_Blended(font, text, 0, OPTION_COLOR);
        SDL_Texture* optionText = SDL_CreateTextureFromSurface(renderer, tempSurface);
        if (!optionText){
            SDL_Log("Could not create option %s texture! Error: %s", text, SDL_GetError());
        }

        mMenuOptions.push_back(std::make_unique<MenuOption>(MenuOption{optionText, callback}));

        SDL_DestroySurface(tempSurface);
    }
};

#endif // __MAIN_MENU_H__