#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <unordered_map>
#include <vector>
#include "input.h"

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;

constexpr int MAX_PLAYER_CONTROLLERS = 2;

const Input::KeyboardConfig PLAYER1_CONFIG = {
    SDL_SCANCODE_W, //UP
    SDL_SCANCODE_S, //DOWN
    SDL_SCANCODE_RETURN, //confirm
    SDL_SCANCODE_ESCAPE, // esc
};

const Input::KeyboardConfig PLAYER2_CONFIG = {
    SDL_SCANCODE_UP, //UP
    SDL_SCANCODE_DOWN, //DOWN
    SDL_SCANCODE_RETURN, //confirm
    SDL_SCANCODE_ESCAPE, // esc
};

Input::InputState gPlayer1InputState;
Input::InputState gPlayer2InputState;

static SDL_Window *gWindow = NULL;
static SDL_Renderer* gRenderer = NULL;

static std::vector<Input*> gActiveInputs;
static std::unordered_map<SDL_JoystickID, Input*> gGamepadToInput;


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Pong", "dev-0.1", "com.github.Deadecho1.Pong");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Pong", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &gWindow, &gRenderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(gRenderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // create space for inputs
    gActiveInputs.reserve(MAX_PLAYER_CONTROLLERS);
    // create player 1
    Input* player1 = new Input(PLAYER1_CONFIG);
    gActiveInputs.push_back(player1);
    player1->SetActive(true);

    Input* player2 = new Input(PLAYER2_CONFIG);
    gActiveInputs.push_back(player2);
    player2->SetActive(false);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT: {
            return SDL_APP_SUCCESS; // end program
            break;
        }

        case SDL_EVENT_GAMEPAD_ADDED: {
            const SDL_JoystickID which = event->gdevice.which;
            // find input with no associated gamepad
            int inputI = 0;
            while (inputI < gActiveInputs.size()){
                if (gActiveInputs[inputI]->GetController() == nullptr){
                    break;
                }
                inputI++;
            }

            // there is an input slot for the controller, connect it.
            if (inputI < gActiveInputs.size()){
                SDL_Gamepad *newController = SDL_OpenGamepad(which);
                if (!newController){
                    SDL_Log("Couldn't open gamepad #%u: %s", (unsigned int) which, SDL_GetError());
                }
                // controller opened and ready to be associated with an input
                else{
                    gGamepadToInput[which] = gActiveInputs[inputI];
                    gActiveInputs[inputI]->SetController(newController);
                }
            }
            break;
        }

        case SDL_EVENT_GAMEPAD_REMOVED: {
            const SDL_JoystickID which = event->gdevice.which;
            
            // find if gamepad was associated wih an input
            auto it = gGamepadToInput.find(which);
            if (it != gGamepadToInput.end()){
                SDL_Gamepad *gamepad = gGamepadToInput[which]->RemoveController();
                if (gamepad){
                    SDL_CloseGamepad(gamepad);
                }
                gGamepadToInput.erase(which); // gamepad is no longer associated with this input
            }

            break;
        }
        
        default: {
            break;
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    // render black background
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(gRenderer);

    SDL_RenderPresent(gRenderer);

    // poll player 1 input
    if (gActiveInputs[0]->InputStatePoll(gPlayer1InputState)){
        if (gPlayer1InputState.up){
            SDL_Log("Up!");
        }
        if (gPlayer1InputState.down){
            SDL_Log("Down!");
        }
        if (gPlayer1InputState.confirm){
            SDL_Log("Confirm!");
        }
        if (gPlayer1InputState.esc){
            SDL_Log("Escape!");
        }
    }
    
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
    // remove inputs
    for (int i = 0; i < gActiveInputs.size(); i++){
        if (gActiveInputs[i] != nullptr){
            delete gActiveInputs[i];
        }
    }

    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);

    gRenderer = NULL;
    gWindow = NULL;
}