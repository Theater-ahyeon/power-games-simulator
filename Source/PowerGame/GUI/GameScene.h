#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <memory>
#include <vector>
#include "GUIRenderer.h"
#include "GUIComponents.h"

namespace PowerGame {

enum class GameState {
    TITLE_SCREEN,
    MAIN_MENU,
    NEW_GAME,
    LOAD_GAME,
    GAMEPLAY,
    PAUSE,
    SETTINGS,
    SAVE_GAME,
    END_GAME,
    EXIT
};

class GameScene {
public:
    GameScene(GUIRenderer* renderer);
    virtual ~GameScene();
    
    virtual void Enter() = 0;
    virtual void Exit() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual bool HandleEvent(const SDL_Event& event) = 0;
    
    GameState GetNextState() const { return nextState_; }
    void SetNextState(GameState state) { nextState_ = state; }

protected:
    GUIRenderer* renderer_;
    GameState nextState_ = GameState::TITLE_SCREEN;
};

class TitleScreenScene : public GameScene {
public:
    TitleScreenScene(GUIRenderer* renderer);
    
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    bool HandleEvent(const SDL_Event& event) override;

private:
    float animationTime_ = 0.0f;
    std::unique_ptr<GUIButton> newGameButton_;
    std::unique_ptr<GUIButton> loadGameButton_;
    std::unique_ptr<GUIButton> settingsButton_;
    std::unique_ptr<GUIButton> exitButton_;
};

class MainMenuScene : public GameScene {
public:
    MainMenuScene(GUIRenderer* renderer);
    
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    bool HandleEvent(const SDL_Event& event) override;

private:
    std::unique_ptr<GUIPanel> resourcePanel_;
    std::unique_ptr<GUIPanel> factionPanel_;
    std::unique_ptr<GUIPanel> actionPanel_;
    std::unique_ptr<GUIPanel> eventPanel_;
};

class GameplayScene : public GameScene {
public:
    GameplayScene(GUIRenderer* renderer);
    
    void Enter() override;
    void Exit() override;
    void Update(float deltaTime) override;
    void Render() override;
    bool HandleEvent(const SDL_Event& event) override;
    
    void ShowDashboard();
    void ShowResourcePanel();
    void ShowFactionPanel();
    void ShowIntelligencePanel();
    void ShowAlliancePanel();
    void ShowCharacterPanel();
    void ShowStoryEvent();

private:
    int currentPanel_ = 0;
    std::vector<std::string> panelNames_ = {
        "DASHBOARD", "RESOURCES", "FACTIONS", 
        "INTELLIGENCE", "ALLIANCES", "CHARACTERS", "STORY"
    };
};

class SceneManager {
public:
    static SceneManager& Instance();
    
    void Initialize(GUIRenderer* renderer);
    void Shutdown();
    
    void ChangeScene(GameState state);
    void Update(float deltaTime);
    void Render();
    bool HandleEvent(const SDL_Event& event);
    
    GameScene* GetCurrentScene() { return currentScene_.get(); }

private:
    SceneManager() = default;
    
    GUIRenderer* renderer_ = nullptr;
    std::unique_ptr<GameScene> currentScene_;
    std::unordered_map<GameState, std::unique_ptr<GameScene>> scenes_;
};

}
