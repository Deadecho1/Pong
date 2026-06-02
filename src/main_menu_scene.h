#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "scene.h"
#include "scene_manager.h"
#include "utilities.h"

static constexpr float COLOR_SPEED = 10.0f;

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
        tempSurface = TTF_RenderText_Blended(appstate->fontMedium, "1-PLAYER", 0, COLOR_WHITE);
        SDL_Texture* optionOneText = SDL_CreateTextureFromSurface(appstate->renderer, tempSurface);
        if (!optionOneText){
            SDL_Log("Could not create option texture! Error: %s", SDL_GetError());
        }
        mMenuOptions.push_back(optionOneText);
        SDL_DestroySurface(tempSurface);

        tempSurface = TTF_RenderText_Blended(appstate->fontMedium, "2-PLAYER", 0, COLOR_WHITE);
        SDL_Texture* optionTwoText = SDL_CreateTextureFromSurface(appstate->renderer, tempSurface);
        if (!optionTwoText){
            SDL_Log("Could not create option texture! Error: %s", SDL_GetError());
        }
        mMenuOptions.push_back(optionTwoText);
        SDL_DestroySurface(tempSurface);

        tempSurface = TTF_RenderText_Blended(appstate->fontMedium, "EXIT", 0, COLOR_WHITE);
        SDL_Texture* optionExitText = SDL_CreateTextureFromSurface(appstate->renderer, tempSurface);
        if (!optionExitText){
            SDL_Log("Could not create option texture! Error: %s", SDL_GetError());
        }
        mMenuOptions.push_back(optionExitText);
        SDL_DestroySurface(tempSurface);

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
                SDL_Log("[PLAYER1] Confirm!");
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
            if (appstate->player2InputState.confirmPressed){
                SDL_Log("[PLAYER2] Confirm!");
            }
            if (appstate->player2InputState.escPressed){
                SDL_Log("[PLAYER2] Escape!");
            }
        }

        mColorTimer += delta * COLOR_SPEED;
        mSelectionColor = LerpColor(COLOR_WHITE, COLOR_YELLOW, (SDL_sinf(mColorTimer) + 1) / 2);
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
            SDL_GetTextureSize(mMenuOptions[i], &tw, &th);
            midX = (mScreenW - tw) / 2;
            midY = (mScreenH - th) / 2;
            SDL_FRect dest = {midX, midY + i * (th + optionsPadding), tw, th};
            // play animation on selected item
            if (mSelectedIndex == i){
                SDL_SetTextureColorMod(mMenuOptions[i], mSelectionColor.r, mSelectionColor.g, mSelectionColor.b);
            }
            else{
                SDL_SetTextureColorMod(mMenuOptions[i], COLOR_WHITE.r, COLOR_WHITE.g, COLOR_WHITE.b);
            }
            SDL_RenderTexture(renderer, mMenuOptions[i], NULL, &dest);
        }

        SDL_RenderPresent(renderer);
    }

    ~MainMenuScene()
    {
        SDL_DestroyTexture(mTitleTexture);
        for (int i = 0; i < mMenuOptions.size(); i++){
            SDL_DestroyTexture(mMenuOptions[i]);
        }
    }
private:
    SceneManager& mSceneManager;

    SDL_Texture* mTitleTexture;

    int mSelectedIndex = 0;
    std::vector<SDL_Texture*> mMenuOptions;

    int mScreenW;
    int mScreenH;

    float mColorTimer = 0.0f;
    SDL_Color mSelectionColor = COLOR_WHITE;
};



#endif // __MAIN_MENU_H__