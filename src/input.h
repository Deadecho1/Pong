#ifndef __INPUT_H__
#define __INPUT_H__

#include <SDL3/SDL.h>

class Input
{
public:
    struct InputState {
        bool up;
        bool down;
        bool confirm;
        bool esc;
    };

    struct KeyboardConfig
    {
        SDL_Scancode up;
        SDL_Scancode down;
        SDL_Scancode confirm;
        SDL_Scancode esc;
    };

    // consts
    static constexpr float DEADZONE = 4000.0f;
    
    Input(const KeyboardConfig &initialConfig);

    ~Input();

    void SetKeyboardInputConfig(const KeyboardConfig &config);

    // gets a const reference to the controller
    const SDL_Gamepad* GetController(); 

    // associates a controller with this input.
    void SetController(SDL_Gamepad* gamepad);

    // call to disassociate controller with input. returns the controller.
    SDL_Gamepad* RemoveController();

    // call in game loop
    bool InputStatePoll(InputState& inputOut);

    bool IsActive();

    void SetActive(bool state);

private:
    SDL_Gamepad *mGamepad = nullptr;

    KeyboardConfig mConfig;

    bool mIsActive = false;
};


Input::Input(const KeyboardConfig &initialConfig)
{
    SetKeyboardInputConfig(initialConfig);
}

Input::~Input()
{
}

const SDL_Gamepad* Input::GetController()
{
    return mGamepad;
}

void Input::SetKeyboardInputConfig(const KeyboardConfig &config)
{
    mConfig = config;
}

void Input::SetController(SDL_Gamepad* gamepad)
{
    mGamepad = gamepad;
}

SDL_Gamepad* Input::RemoveController()
{
    SDL_Gamepad *g = mGamepad;
    mGamepad = nullptr;
    return g;
}

bool Input::IsActive(){
    return mIsActive;
}

void Input::SetActive(bool state){
    mIsActive = state;
}

// returns false if input is inactive
bool Input::InputStatePoll(Input::InputState& inputOut)
{
    if (!mIsActive){
        return false;
    }

    // gamepad associated with this input
    if (mGamepad != nullptr)
    {         
        inputOut.esc = SDL_GetGamepadButton(mGamepad, SDL_GAMEPAD_BUTTON_START);
        inputOut.confirm = SDL_GetGamepadButton(mGamepad, SDL_GAMEPAD_BUTTON_SOUTH);

        Sint16 axis_y = SDL_GetGamepadAxis(mGamepad, SDL_GAMEPAD_AXIS_LEFTY);
        inputOut.down = (axis_y > Input::DEADZONE || SDL_GetGamepadButton(mGamepad,SDL_GAMEPAD_BUTTON_DPAD_DOWN));
        inputOut.up = (axis_y < -Input::DEADZONE || SDL_GetGamepadButton(mGamepad, SDL_GAMEPAD_BUTTON_DPAD_UP));
    }
    // no gamepad, default to keyboard controls
    else
    {
        const bool *key_states = SDL_GetKeyboardState(NULL);

        inputOut.esc = (key_states[mConfig.esc]);
        inputOut.confirm = (key_states[mConfig.confirm]);
        inputOut.up = (key_states[mConfig.up]);
        inputOut.down = (key_states[mConfig.down]);
    }
    
    return true;
}

#endif // __INPUT_H__