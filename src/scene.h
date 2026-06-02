#ifndef __SCENE_H__
#define __SCENE_H__

#include "SDL3/SDL.h"

struct App;

class Scene
{
    public:
        virtual ~Scene() = default;
        virtual void OnEnter() {}
        virtual void OnExit() {}
        virtual void Init(App* appstate) {}
        virtual void OnEvent(App* appstate, SDL_Event *event) = 0;
        virtual void Update(App *appstate, float delta) = 0;
        virtual void Render(SDL_Renderer* renderer) = 0;
};

#endif // __SCENE_H__