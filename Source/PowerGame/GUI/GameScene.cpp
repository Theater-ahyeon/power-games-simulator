#include "GameScene.h"
#include "GUIRenderer.h"
#include <algorithm>

namespace PowerGame {

GameScene::GameScene(GUIRenderer* renderer) : renderer_(renderer) {}
GameScene::~GameScene() = default;

TitleScreenScene::TitleScreenScene(GUIRenderer* renderer)
    : GameScene(renderer) {}

void TitleScreenScene::Enter() {
    int centerX = renderer_->GetWidth() / 2;
    int buttonWidth = 200;
    int buttonHeight = 50;
    int buttonSpacing = 70;
    int startY = 400;
    
    newGameButton_ = std::make_unique<GUIButton>(
        centerX - buttonWidth / 2, startY,
        buttonWidth, buttonHeight, "NEW GAME"
    );
    newGameButton_->SetClickHandler([this]() {
        SetNextState(GameState::NEW_GAME);
    });
    
    loadGameButton_ = std::make_unique<GUIButton>(
        centerX - buttonWidth / 2, startY + buttonSpacing,
        buttonWidth, buttonHeight, "LOAD GAME"
    );
    loadGameButton_->SetClickHandler([this]() {
        SetNextState(GameState::LOAD_GAME);
    });
    
    settingsButton_ = std::make_unique<GUIButton>(
        centerX - buttonWidth / 2, startY + buttonSpacing * 2,
        buttonWidth, buttonHeight, "SETTINGS"
    );
    settingsButton_->SetClickHandler([this]() {
        SetNextState(GameState::SETTINGS);
    });
    
    exitButton_ = std::make_unique<GUIButton>(
        centerX - buttonWidth / 2, startY + buttonSpacing * 3,
        buttonWidth, buttonHeight, "EXIT"
    );
    exitButton_->SetClickHandler([this]() {
        SetNextState(GameState::EXIT);
    });
}

void TitleScreenScene::Exit() {}

void TitleScreenScene::Update(float deltaTime) {
    animationTime_ += deltaTime;
}

void TitleScreenScene::Render() {
    renderer_->Clear();
    
    int centerX = renderer_->GetWidth() / 2;
    renderer_->DrawText("POWER GAME", centerX - 150, 150, nullptr, Color::Primary);
    renderer_->DrawText("The Art of Political Intrigue", centerX - 200, 200, nullptr, Color::TextSecondary);
    
    renderer_->SetDrawColor(Color::Outline);
    renderer_->DrawLine(100, 300, renderer_->GetWidth() - 100, 300);
    
    newGameButton_->Render(renderer_);
    loadGameButton_->Render(renderer_);
    settingsButton_->Render(renderer_);
    exitButton_->Render(renderer_);
    
    renderer_->Present();
}

bool TitleScreenScene::HandleEvent(const SDL_Event& event) {
    newGameButton_->HandleEvent(event);
    loadGameButton_->HandleEvent(event);
    settingsButton_->HandleEvent(event);
    exitButton_->HandleEvent(event);
    
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_1:
                SetNextState(GameState::NEW_GAME);
                return true;
            case SDLK_2:
                SetNextState(GameState::LOAD_GAME);
                return true;
            case SDLK_ESCAPE:
                SetNextState(GameState::EXIT);
                return true;
        }
    }
    
    return false;
}

MainMenuScene::MainMenuScene(GUIRenderer* renderer)
    : GameScene(renderer) {}

void MainMenuScene::Enter() {
    int panelWidth = 300;
    int panelHeight = 600;
    int margin = 20;
    
    resourcePanel_ = std::make_unique<GUIPanel>(margin, margin, panelWidth, panelHeight);
    factionPanel_ = std::make_unique<GUIPanel>(margin * 2 + panelWidth, margin, panelWidth, panelHeight);
    actionPanel_ = std::make_unique<GUIPanel>(margin * 3 + panelWidth * 2, margin, panelWidth, panelHeight);
    eventPanel_ = std::make_unique<GUIPanel>(margin * 4 + panelWidth * 3, margin, 
        renderer_->GetWidth() - margin * 5 - panelWidth * 3, panelHeight);
}

void MainMenuScene::Exit() {}

void MainMenuScene::Update(float deltaTime) {}

void MainMenuScene::Render() {
    renderer_->Clear();
    
    renderer_->DrawText("RESOURCES", 30, 30, nullptr, Color::Primary);
    renderer_->DrawText("FACTIONS", 330, 30, nullptr, Color::Primary);
    renderer_->DrawText("ACTIONS", 630, 30, nullptr, Color::Primary);
    renderer_->DrawText("EVENTS", 930, 30, nullptr, Color::Primary);
    
    resourcePanel_->Render(renderer_);
    factionPanel_->Render(renderer_);
    actionPanel_->Render(renderer_);
    eventPanel_->Render(renderer_);
    
    renderer_->Present();
}

bool MainMenuScene::HandleEvent(const SDL_Event& event) {
    return false;
}

GameplayScene::GameplayScene(GUIRenderer* renderer)
    : GameScene(renderer) {}

void GameplayScene::Enter() {
    currentPanel_ = 0;
}

void GameplayScene::Exit() {}

void GameplayScene::Update(float deltaTime) {}

void GameplayScene::Render() {
    renderer_->Clear();
    
    int topBarHeight = 50;
    renderer_->SetDrawColor(Color::SurfaceLow);
    renderer_->DrawRect(0, 0, renderer_->GetWidth(), topBarHeight, true);
    
    renderer_->DrawText("TURN: 12/30", 20, 15, nullptr, Color::TextPrimary);
    renderer_->DrawText("PHASE: ACTION", 200, 15, nullptr, Color::Primary);
    
    int sidebarWidth = 200;
    int sidebarX = renderer_->GetWidth() - sidebarWidth;
    renderer_->SetDrawColor(Color::SurfaceLow);
    renderer_->DrawRect(sidebarX, topBarHeight, sidebarWidth, 
        renderer_->GetHeight() - topBarHeight, true);
    
    for (size_t i = 0; i < panelNames_.size(); ++i) {
        int y = topBarHeight + 20 + static_cast<int>(i) * 50;
        bool isSelected = (static_cast<int>(i) == currentPanel_);
        
        if (isSelected) {
            renderer_->SetDrawColor(Color::Primary);
            renderer_->DrawRect(sidebarX + 10, y, sidebarWidth - 20, 40, true);
        }
        
        renderer_->DrawText(panelNames_[i], sidebarX + 20, y + 10, nullptr, 
            isSelected ? Color::Background : Color::TextPrimary);
    }
    
    renderer_->Present();
}

bool GameplayScene::HandleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_1: currentPanel_ = 0; return true;
            case SDLK_2: currentPanel_ = 1; return true;
            case SDLK_3: currentPanel_ = 2; return true;
            case SDLK_4: currentPanel_ = 3; return true;
            case SDLK_5: currentPanel_ = 4; return true;
            case SDLK_6: currentPanel_ = 5; return true;
            case SDLK_7: currentPanel_ = 6; return true;
            case SDLK_ESCAPE:
                SetNextState(GameState::PAUSE);
                return true;
        }
    }
    
    return false;
}

void GameplayScene::ShowDashboard() { currentPanel_ = 0; }
void GameplayScene::ShowResourcePanel() { currentPanel_ = 1; }
void GameplayScene::ShowFactionPanel() { currentPanel_ = 2; }
void GameplayScene::ShowIntelligencePanel() { currentPanel_ = 3; }
void GameplayScene::ShowAlliancePanel() { currentPanel_ = 4; }
void GameplayScene::ShowCharacterPanel() { currentPanel_ = 5; }
void GameplayScene::ShowStoryEvent() { currentPanel_ = 6; }

SceneManager& SceneManager::Instance() {
    static SceneManager instance;
    return instance;
}

void SceneManager::Initialize(GUIRenderer* renderer) {
    renderer_ = renderer;
    scenes_[GameState::TITLE_SCREEN] = std::make_unique<TitleScreenScene>(renderer);
    scenes_[GameState::MAIN_MENU] = std::make_unique<MainMenuScene>(renderer);
    scenes_[GameState::GAMEPLAY] = std::make_unique<GameplayScene>(renderer);
    currentScene_ = std::move(scenes_[GameState::TITLE_SCREEN]);
    currentScene_->Enter();
}

void SceneManager::Shutdown() {
    if (currentScene_) {
        currentScene_->Exit();
    }
    scenes_.clear();
}

void SceneManager::ChangeScene(GameState state) {
    if (currentScene_) {
        currentScene_->Exit();
    }
    
    if (scenes_.find(state) == scenes_.end()) {
        scenes_[state] = std::make_unique<TitleScreenScene>(renderer_);
    }
    
    currentScene_ = std::move(scenes_[state]);
    currentScene_->Enter();
}

void SceneManager::Update(float deltaTime) {
    if (currentScene_) {
        currentScene_->Update(deltaTime);
        
        GameState nextState = currentScene_->GetNextState();
        if (nextState != GameState::TITLE_SCREEN) {
            ChangeScene(nextState);
        }
    }
}

void SceneManager::Render() {
    if (currentScene_) {
        currentScene_->Render();
    }
}

bool SceneManager::HandleEvent(const SDL_Event& event) {
    if (currentScene_) {
        return currentScene_->HandleEvent(event);
    }
    return false;
}

}
