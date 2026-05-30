#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include <vector>
#include <memory>
#include "scene.h"

class SceneManager
{
    public:
        void PushScene(std::unique_ptr<Scene> scene) {
            scene->Init();
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
#endif // __SCENE_MANAGER_H__