#pragma once

#include <cmath>
#include "SDL3/SDL.h"

constexpr SDL_Color COLOR_WHITE = {255, 255, 255, 255};
constexpr SDL_Color COLOR_BLACK = {0, 0, 0, 0};
constexpr SDL_Color COLOR_YELLOW = {255, 255, 0, 255};

SDL_Color LerpColor(SDL_Color a, SDL_Color b, float t){
    return {
        (Uint8)(a.r + (b.r - a.r) * t),
        (Uint8)(a.g + (b.g - a.g) * t),
        (Uint8)(a.b + (b.b - a.b) * t),
        (Uint8)(a.a + (b.a - a.a) * t)
    };
}

class Vector2D
{
public:
    Vector2D(float x, float y) : x(x), y(y) {}
    Vector2D() : x(0), y(0) {}

    float x, y;

    float LengthSquared(){ return x*x + y*y; }

    float Length() { return std::sqrt(LengthSquared()); }

    Vector2D Normalized(){
        float l = Length();
        if (l == 0.0) { return Vector2D(0.0, 0.0); }
        return Vector2D(x/l, y/l);
    }
};