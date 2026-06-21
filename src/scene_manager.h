#pragma once

#include <vector>
#include <memory>
#include "scene.h"
#include "app.h"

class SceneManager
{
    public:
        void PushScene(std::unique_ptr<Scene> scene, App* appstate) {
            scene->Init(appstate);
            if(!mSceneStack.empty()){
                mSceneStack.back()->OnExit();
            }
            mSceneStack.push_back(std::move(scene));
            mSceneStack.back()->OnEnter();
        }

        void PopScene() {
            mSceneStack.back()->OnExit();
            mSceneStack.pop_back();
            mSceneStack.back()->OnEnter();
        }

        Scene& CurrentScene() {
            SDL_assert(!mSceneStack.empty());
            return *mSceneStack.back();
        }

    private:
        std::vector<std::unique_ptr<Scene>> mSceneStack;
};