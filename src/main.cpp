#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <unordered_map>
#include <vector>
#include "input.h"
#include "app.h"
#include "main_menu_scene.h"

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 480;

constexpr int MAX_PLAYER_CONTROLLERS = 2;

constexpr int FONT_SIZE_SMALL = 16;
constexpr int FONT_SIZE_MEDIUM = 32;
constexpr int FONT_SIZE_LARGE = 64;

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


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    App* app = new App();
    SDL_SetAppMetadata("Pong", "dev-0.1", "com.github.Deadecho1.Pong");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init()){
        SDL_Log("Couldn't initialize TTF: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Pong", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &app->window, &app->renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(app->renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // load fonts
    app->fontSmall = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", FONT_SIZE_SMALL);
    if (!app->fontSmall){
        SDL_Log("Couldn't open small font: %s", SDL_GetError());
    }
    app->fontMedium = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", FONT_SIZE_MEDIUM);
    if (!app->fontMedium){
        SDL_Log("Couldn't open medium font: %s", SDL_GetError());
    }
    app->fontLarge = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", FONT_SIZE_LARGE);
    if (!app->fontLarge){
        SDL_Log("Couldn't open large font: %s", SDL_GetError());
    }

    app->screenW = WINDOW_WIDTH;
    app->screenH = WINDOW_HEIGHT;

    // create player 1
    std::unique_ptr<Input> player1 = std::make_unique<Input>(PLAYER1_CONFIG);
    player1->SetActive(true);
    app->activeInputs.push_back(std::move(player1));

    // create player 2
    std::unique_ptr<Input> player2 = std::make_unique<Input>(PLAYER2_CONFIG);
    player2->SetActive(true);
    app->activeInputs.push_back(std::move(player2));
    
    // initialize first scene
    std::unique_ptr<Scene> initialScene = std::make_unique<MainMenuScene>(app->sceneManager);
    app->sceneManager.PushScene(std::move(initialScene), app);

    app->last_tick = SDL_GetTicks();

    *appstate = app;
    
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    App* app = static_cast<App*>(appstate);
    Scene& currScene = app->sceneManager.CurrentScene();
    
    switch (event->type)
    {
        case SDL_EVENT_QUIT: {
            return SDL_APP_SUCCESS; // end program
            break;
        }

        case SDL_EVENT_GAMEPAD_ADDED: {
            const SDL_JoystickID which = event->gdevice.which;
            // find input with no associated gamepad, prioritizing player 2 input
            int inputI = 0;
            int activeInputsNum = app->activeInputs.size();
            while (inputI < activeInputsNum){
                if (app->activeInputs[(inputI+1) % MAX_PLAYER_CONTROLLERS]->GetController() == nullptr){
                    break;
                }
                inputI++;
            }

            // there is an input slot for the controller, connect it.
            if (inputI < activeInputsNum){
                SDL_Gamepad *newController = SDL_OpenGamepad(which);
                if (!newController){
                    SDL_Log("Couldn't open gamepad #%u: %s", (unsigned int) which, SDL_GetError());
                }
                // controller opened and ready to be associated with an input
                else{
                    app->gamepadToInput[which] = app->activeInputs[(inputI+1) % MAX_PLAYER_CONTROLLERS].get();
                    app->activeInputs[(inputI+1) % MAX_PLAYER_CONTROLLERS]->SetController(newController);
                }
            }
            break;
        }

        case SDL_EVENT_GAMEPAD_REMOVED: {
            const SDL_JoystickID which = event->gdevice.which;
            
            // find if gamepad was associated wih an input
            auto it = app->gamepadToInput.find(which);
            if (it != app->gamepadToInput.end()){
                SDL_Gamepad *gamepad = app->gamepadToInput[which]->RemoveController();
                if (gamepad){
                    SDL_CloseGamepad(gamepad);
                }
                app->gamepadToInput.erase(which); // gamepad is no longer associated with this input
            }

            break;
        }
        
        default: {
            break;
        }
    }

    currScene.OnEvent(app, event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
    App* app = static_cast<App*>(appstate);
    
    Scene& currScene = app->sceneManager.CurrentScene();

    Uint64 now = SDL_GetTicks();
    float delta = (now - app->last_tick) / 1000.0f;
    app->last_tick = now;

    currScene.Update(app, delta);
    currScene.Render(app->renderer);
    
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result){
    App* app = static_cast<App*>(appstate);

    TTF_CloseFont(app->fontSmall);
    TTF_CloseFont(app->fontMedium);
    TTF_CloseFont(app->fontLarge);
    TTF_Quit();

    // close gamepads
    for (auto& [id, input] : app->gamepadToInput){
        SDL_Gamepad* gamepad = input->RemoveController();
        if (gamepad) SDL_CloseGamepad(gamepad);
    }

    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);

    app->renderer = nullptr;
    app->window = nullptr;

    delete app;
}