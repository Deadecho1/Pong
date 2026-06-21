#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#include "utilities.h"

class Texture {
public:
    Texture() : mTexture(nullptr), mPath("") {}

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    ~Texture() {
        if (mTexture != nullptr) SDL_DestroyTexture(mTexture);
    }

    bool LoadFromPath(SDL_Renderer* renderer, std::string path){
        if (mTexture != nullptr){
            SDL_DestroyTexture(mTexture);
        }

        mTexture = IMG_LoadTexture(renderer, path.c_str());
        mPath = path;
        
        return mTexture != nullptr;
    }

    bool LoadText(SDL_Renderer* renderer, std::string txt, TTF_Font* font, SDL_Color color = COLOR_WHITE){
        if (mTexture != nullptr){
            SDL_DestroyTexture(mTexture);
        }

        SDL_Surface* temp = TTF_RenderText_Blended(font, txt.c_str(), txt.size(), color);
        mTexture = SDL_CreateTextureFromSurface(renderer, temp);
        SDL_DestroySurface(temp);

        return mTexture != nullptr;
    }

    void Render(SDL_Renderer* renderer, SDL_FRect &dest){
        SDL_RenderTexture(renderer, mTexture, NULL, &dest);
    }

    void GetSize(float &tw, float &th){
        if(mTexture != nullptr){
            SDL_GetTextureSize(mTexture, &tw, &th);
        }
    }

    void SetColorMod(Uint8 r, Uint8 g, Uint8 b){
        SDL_SetTextureColorMod(mTexture, r, g, b);
    }

    bool IsLoaded(){
        return mTexture != nullptr;
    }

private:
    SDL_Texture* mTexture;
    std::string mPath;
};