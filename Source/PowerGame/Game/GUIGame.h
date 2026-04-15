#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <memory>
#include <chrono>
#include "../Core/GameManager.h"
#include "../Resources/ResourceManager.h"
#include "../Faction/FactionManager.h"
#include "../Intelligence/IntelligenceManager.h"
#include "../Alliance/AllianceManager.h"
#include "../Character/CharacterManager.h"
#include "../Story/StoryManager.h"
#include "../Strategy/StrategyManager.h"
#include "../Ending/EndingManager.h"
#include "../Data/SaveManager.h"
#include "../AI/AIDecisionEngine.h"
#include "../GUI/GUIRenderer.h"
#include "../GUI/GUIComponents.h"
#include "../GUI/GameScene.h"
#include "../Audio/AudioManager.h"

namespace PowerGame {

class GUIGame {
public:
    static GUIGame& Instance() {
        static GUIGame instance;
        return instance;
    }
    
    int Run() {
        if (!Initialize()) {
            return 1;
        }
        
        GameLoop();
        Shutdown();
        
        return 0;
    }

private:
    GUIGame() = default;
    
    bool Initialize() {
        if (!renderer_.Initialize(1280, 720)) {
            std::cerr << "Failed to initialize GUI renderer" << std::endl;
            return false;
        }
        
        if (!AudioManager::Instance().Initialize()) {
            std::cerr << "Warning: Audio initialization failed, continuing without audio" << std::endl;
        }
        
        auto& gameMgr = GameManager::Instance();
        gameMgr.Initialize();
        
        SceneManager::Instance().Initialize(&renderer_);
        
        running_ = true;
        return true;
    }
    
    void Shutdown() {
        SceneManager::Instance().Shutdown();
        AudioManager::Instance().Shutdown();
        renderer_.Shutdown();
        running_ = false;
    }
    
    void GameLoop() {
        auto lastTime = std::chrono::high_resolution_clock::now();
        const float targetFPS = 60.0f;
        const float frameTime = 1.0f / targetFPS;
        
        while (running_) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            ProcessEvents();
            
            SceneManager::Instance().Update(deltaTime);
            SceneManager::Instance().Render();
            
            auto frameEnd = std::chrono::high_resolution_clock::now();
            float elapsed = std::chrono::duration<float>(frameEnd - currentTime).count();
            if (elapsed < frameTime) {
                SDL_Delay(static_cast<Uint32>((frameTime - elapsed) * 1000));
            }
        }
    }
    
    void ProcessEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running_ = false;
            }
            
            SceneManager::Instance().HandleEvent(event);
        }
    }
    
    bool running_ = false;
    GUIRenderer renderer_;
};

}
