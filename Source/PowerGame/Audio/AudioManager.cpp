#include "AudioManager.h"
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <cmath>

namespace PowerGame {

AudioManager& AudioManager::Instance() {
    static AudioManager instance;
    return instance;
}

bool AudioManager::Initialize() {
    if (initialized_) return true;
    
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL Audio: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
        std::cerr << "Failed to open audio: " << Mix_GetError() << std::endl;
        return false;
    }
    
    Mix_VolumeMusic(static_cast<int>(musicVolume_ * 128));
    Mix_Volume(-1, static_cast<int>(sfxVolume_ * 128));
    
    initialized_ = true;
    return true;
}

void AudioManager::Shutdown() {
    if (!initialized_) return;
    
    for (auto& pair : soundEffects_) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    soundEffects_.clear();
    
    for (auto& pair : musicTracks_) {
        if (pair.second) {
            Mix_FreeMusic(pair.second);
        }
    }
    musicTracks_.clear();
    
    Mix_CloseAudio();
    initialized_ = false;
}

void AudioManager::PlayMusic(MusicTrack track, int loops) {
    if (!initialized_) return;
    
    auto it = musicTracks_.find(track);
    if (it != musicTracks_.end() && it->second) {
        Mix_PlayMusic(it->second, loops);
    }
}

void AudioManager::StopMusic() {
    if (initialized_) {
        Mix_HaltMusic();
    }
}

void AudioManager::PauseMusic() {
    if (initialized_) {
        Mix_PauseMusic();
    }
}

void AudioManager::ResumeMusic() {
    if (initialized_) {
        Mix_ResumeMusic();
    }
}

void AudioManager::SetMusicVolume(float volume) {
    musicVolume_ = std::max(0.0f, std::min(1.0f, volume));
    if (initialized_) {
        Mix_VolumeMusic(static_cast<int>(musicVolume_ * 128));
    }
}

void AudioManager::PlaySoundEffect(SoundEffect effect) {
    if (!initialized_) return;
    
    auto it = soundEffects_.find(effect);
    if (it != soundEffects_.end() && it->second) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void AudioManager::SetSoundEffectVolume(float volume) {
    sfxVolume_ = std::max(0.0f, std::min(1.0f, volume));
    if (initialized_) {
        Mix_Volume(-1, static_cast<int>(sfxVolume_ * 128));
    }
}

bool AudioManager::LoadSoundEffect(SoundEffect effect, const std::string& filePath) {
    if (!initialized_) return false;
    
    Mix_Chunk* chunk = Mix_LoadWAV(filePath.c_str());
    if (!chunk) {
        std::cerr << "Failed to load sound effect: " << filePath << std::endl;
        return false;
    }
    
    soundEffects_[effect] = chunk;
    return true;
}

bool AudioManager::LoadMusic(MusicTrack track, const std::string& filePath) {
    if (!initialized_) return false;
    
    Mix_Music* music = Mix_LoadMUS(filePath.c_str());
    if (!music) {
        std::cerr << "Failed to load music: " << filePath << std::endl;
        return false;
    }
    
    musicTracks_[track] = music;
    return true;
}

void AudioManager::GenerateTTS(const std::string& text, const std::string& outputPath) {
    std::cout << "TTS generation requested: " << text << std::endl;
    std::cout << "Note: TTS requires MiniMax API integration" << std::endl;
}

}
