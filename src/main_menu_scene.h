#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "scene.h"
#include "scene_manager.h"

class MainMenuScene : public Scene {
public:
    explicit MainMenuScene(SceneManager& scene_manager) : mSceneManager(scene_manager) {}

    void OnEvent(App* appstate, SDL_Event *event) override {

    }

    void Update(App* appstate, float delta) override {
        // poll player 1 input
        if (appstate->activeInputs[0]->InputStatePoll(appstate->player1InputState)){
            if (appstate->player1InputState.up){
                SDL_Log("[PLAYER1] Up!");
            }
            if (appstate->player1InputState.down){
                SDL_Log("[PLAYER1] Down!");
            }
            if (appstate->player1InputState.confirm){
                SDL_Log("[PLAYER1] Confirm!");
            }
            if (appstate->player1InputState.esc){
                SDL_Log("[PLAYER1] Escape!");
            }
        }

        // poll player2 input
        if (appstate->activeInputs[1]->InputStatePoll(appstate->player2InputState)){
            if (appstate->player2InputState.up){
                SDL_Log("[PLAYER2] Up!");
            }
            if (appstate->player2InputState.down){
                SDL_Log("[PLAYER2] Down!");
            }
            if (appstate->player2InputState.confirm){
                SDL_Log("[PLAYER2] Confirm!");
            }
            if (appstate->player2InputState.esc){
                SDL_Log("[PLAYER2] Escape!");
            }
        }
    }

    void Render(SDL_Renderer* renderer) override {
        // render black background
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);
    }
private:
    SceneManager& mSceneManager;
};



#endif // __MAIN_MENU_H__