#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "input.h"
#include "scene_manager.h"

static const std::string AUDIO_PATH = "assets/audio/";

struct App{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    int screenW;
    int screenH;

    TTF_Font* fontSmall  = nullptr;
    TTF_Font* fontMedium = nullptr; 
    TTF_Font* fontLarge  = nullptr;

    MIX_Mixer* mixer = nullptr;

    Uint64 last_tick;

    Input::InputState player1InputState;
    Input::InputState player2InputState;

    std::vector<std::unique_ptr<Input>> activeInputs;
    std::unordered_map<SDL_JoystickID, Input*> gamepadToInput;

    SceneManager sceneManager;

    bool shouldExit = false;
};