#pragma once
#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace PowerGame {

enum class SoundEffect {
    BUTTON_CLICK,
    MENU_SELECT,
    ALERT_WARNING,
    ALERT_CRITICAL,
    SUCCESS,
    FAILURE,
    NOTIFICATION,
    DOOR_OPEN,
    DOOR_CLOSE,
    TYPEWRITER,
    AMBIENT_TENSION,
    AMBIENT_CALM
};

enum class MusicTrack {
    TITLE_SCREEN,
    MAIN_MENU,
    GAMEPLAY_CALM,
    GAMEPLAY_TENSE,
    CRISIS,
    VICTORY,
    DEFEAT,
    INTRO_NARRATION
};

class AudioManager {
public:
    static AudioManager& Instance();
    
    bool Initialize();
    void Shutdown();
    
    void PlayMusic(MusicTrack track, int loops = -1);
    void StopMusic();
    void PauseMusic();
    void ResumeMusic();
    void SetMusicVolume(float volume);
    
    void PlaySoundEffect(SoundEffect effect);
    void SetSoundEffectVolume(float volume);
    
    bool LoadSoundEffect(SoundEffect effect, const std::string& filePath);
    bool LoadMusic(MusicTrack track, const std::string& filePath);
    
    void GenerateTTS(const std::string& text, const std::string& outputPath);

private:
    AudioManager() = default;
    ~AudioManager() = default;
    
    float musicVolume_ = 0.7f;
    float sfxVolume_ = 0.8f;
    
    std::unordered_map<SoundEffect, Mix_Chunk*> soundEffects_;
    std::unordered_map<MusicTrack, Mix_Music*> musicTracks_;
    
    bool initialized_ = false;
};

class ProceduralAudioGenerator {
public:
    static void GenerateAmbientTension(const std::string& outputPath, int durationSeconds = 10);
    static void GenerateAmbientCalm(const std::string& outputPath, int durationSeconds = 10);
    static void GenerateAlertSound(const std::string& outputPath, bool critical = false);
    static void GenerateClickSound(const std::string& outputPath);
};

}
