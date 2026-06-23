#pragma once

#include <string>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

class AudioClip{
public:
    AudioClip() : mAudioClip(nullptr) {}

    AudioClip(const AudioClip&) = delete;
    AudioClip& operator=(const AudioClip&) = delete;

    ~AudioClip() {
        if (mAudioClip != nullptr) {
            MIX_DestroyAudio(mAudioClip);
        }
    }

    bool LoadClip(MIX_Mixer* mixer, std::string path)
    {
        mAudioClip = MIX_LoadAudio(mixer, path.c_str(), false);
        if (!mAudioClip){
            SDL_Log("Could not load audio clip %s, error: %s", path, SDL_GetError());
            return false;
            
        }
        
        mMixer = mixer;
        return true;
    }

    void Play(){
        if (mAudioClip && mMixer){
            MIX_PlayAudio(mMixer, mAudioClip);
        }
    }

private:
    MIX_Audio* mAudioClip;
    MIX_Mixer* mMixer;
};