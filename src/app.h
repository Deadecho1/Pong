#pragma once
#include <vector>
#include <unordered_map>
#include "SDL3/SDL.h"
#include "input.h"
#include "scene_manager.h"

struct App{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    Uint64 last_tick;

    Input::InputState player1InputState;
    Input::InputState player2InputState;

    std::vector<std::unique_ptr<Input>> activeInputs;
    std::unordered_map<SDL_JoystickID, Input*> gamepadToInput;

    SceneManager sceneManager;
};