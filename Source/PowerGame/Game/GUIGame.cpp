#include "GUIGame.h"
#include "../GUI/MainMenuScene.h"
#include "../GUI/GameHUD.h"
#include "../GUI/CharacterPanel.h"
#include "../GUI/StoryEventPanel.h"
#include "../GUI/FactionPanel.h"
#include "../GUI/IntelligencePanel.h"
#include "../GUI/SaveLoadPanel.h"
#include "../GUI/TutorialSystem.h"
#include "../Data/SaveManager.h"
#include <iostream>

namespace PowerGame {

class SceneManager {
public:
    static SceneManager& Instance() {
        static SceneManager instance;
        return instance;
    }

    void Initialize(GUIRenderer* renderer) {
        renderer_ = renderer;

        mainMenu_ = std::make_unique<MainMenuScene>(renderer);
        mainMenu_->Initialize();
        mainMenu_->SetNewGameCallback([this]() { StartNewGame(); });
        mainMenu_->SetLoadGameCallback([this]() { LoadGame(); });
        mainMenu_->SetSettingsCallback([this]() { OpenSettings(); });
        mainMenu_->SetQuitCallback([this]() { QuitGame(); });

        gameHUD_ = std::make_unique<GameHUD>(renderer);
        gameHUD_->Initialize();
        gameHUD_->SetMenuCallback([this]() { OpenMenu(); });
        gameHUD_->SetEndTurnCallback([this]() { EndTurn(); });
        gameHUD_->SetCharacterCallback([this]() { OpenCharacterPanel(); });
        gameHUD_->SetFactionCallback([this]() { OpenFactionPanel(); });
        gameHUD_->SetIntelligenceCallback([this]() { OpenIntelPanel(); });

        characterPanel_ = std::make_unique<CharacterPanel>(renderer);
        characterPanel_->Initialize();
        characterPanel_->SetCloseCallback([this]() { CloseCharacterPanel(); });

        storyPanel_ = std::make_unique<StoryEventPanel>(renderer);
        storyPanel_->Initialize();
        storyPanel_->SetChoiceCallback([this](const std::string& nodeId, const std::string& choiceId) {
            MakeStoryChoice(nodeId, choiceId);
        });

        factionPanel_ = std::make_unique<FactionPanel>(renderer);
        factionPanel_->Initialize();
        factionPanel_->SetCloseCallback([this]() { CloseFactionPanel(); });
        factionPanel_->SetFormAllianceCallback([this](const std::string& factionId) {
            FormAlliance(factionId);
        });

        intelPanel_ = std::make_unique<IntelligencePanel>(renderer);
        intelPanel_->Initialize();
        intelPanel_->SetCloseCallback([this]() { CloseIntelPanel(); });
        intelPanel_->SetDeploySpyCallback([this]() { DeploySpy(); });

        saveLoadPanel_ = std::make_unique<SaveLoadPanel>(renderer, false);
        saveLoadPanel_->Initialize();
        saveLoadPanel_->SetCloseCallback([this]() { CloseSaveLoadPanel(); });
        saveLoadPanel_->SetLoadCallback([this](const std::string& saveId) {
            LoadGameFromSave(saveId);
        });

        ShowMainMenu();
    }

    void Shutdown() {
        mainMenu_.reset();
        gameHUD_.reset();
        characterPanel_.reset();
        storyPanel_.reset();
        factionPanel_.reset();
        intelPanel_.reset();
        saveLoadPanel_.reset();
    }

    void Update(float deltaTime) {
        if (gameState_ == GameState::Playing) {
            UpdateGameLogic(deltaTime);
        }
    }

    void Render() {
        renderer_->Clear();

        if (mainMenu_ && mainMenu_->IsVisible()) {
            mainMenu_->Render();
        }

        if (gameHUD_ && gameHUD_->IsVisible()) {
            gameHUD_->Render();
        }

        if (characterPanel_ && characterPanel_->IsVisible()) {
            characterPanel_->Render();
        }

        if (storyPanel_ && storyPanel_->IsVisible()) {
            storyPanel_->Render();
        }

        renderer_->Present();
    }

    void HandleEvent(const SDL_Event& event) {
        if (saveLoadPanel_ && saveLoadPanel_->IsVisible() && saveLoadPanel_->HandleEvent(event)) return;
        if (intelPanel_ && intelPanel_->IsVisible() && intelPanel_->HandleEvent(event)) return;
        if (factionPanel_ && factionPanel_->IsVisible() && factionPanel_->HandleEvent(event)) return;
        if (storyPanel_ && storyPanel_->IsVisible() && storyPanel_->HandleEvent(event)) return;
        if (characterPanel_ && characterPanel_->IsVisible() && characterPanel_->HandleEvent(event)) return;
        if (gameHUD_ && gameHUD_->IsVisible() && gameHUD_->HandleEvent(event)) return;
        if (mainMenu_ && mainMenu_->IsVisible() && mainMenu_->HandleEvent(event)) return;
    }

    void ShowMainMenu() {
        gameState_ = GameState::MainMenu;
        if (mainMenu_) mainMenu_->Show();
        if (gameHUD_) gameHUD_->Hide();
        if (characterPanel_) characterPanel_->Hide();
        if (storyPanel_) storyPanel_->Hide();
    }

    void StartNewGame() {
        GameManager::Instance().StartNewGame();
        gameState_ = GameState::Playing;

        if (mainMenu_) mainMenu_->Hide();
        if (gameHUD_) {
            gameHUD_->Show();
            UpdateHUD();
        }

        TutorialSystem::Instance().StartTutorial();
    }

    void LoadGame() {
        if (saveLoadPanel_) {
            saveLoadPanel_->Show();
        }
    }

    void LoadGameFromSave(const std::string& saveId) {
        if (SaveManager::Instance().LoadGame(saveId)) {
            gameState_ = GameState::Playing;
            if (mainMenu_) mainMenu_->Hide();
            if (gameHUD_) {
                gameHUD_->Show();
                UpdateHUD();
            }
            if (saveLoadPanel_) saveLoadPanel_->Hide();
        }
    }

    void CloseSaveLoadPanel() {
        if (saveLoadPanel_) saveLoadPanel_->Hide();
    }

    void OpenSettings() {
        // TODO: Implement settings panel
    }

    void QuitGame() {
        running_ = false;
    }

    void OpenMenu() {
        ShowMainMenu();
    }

    void EndTurn() {
        GameManager::Instance().AdvanceTurn();
        UpdateHUD();
        CheckStoryEvents();
    }

    void OpenCharacterPanel() {
        if (characterPanel_) {
            auto* charMgr = GameSystemManager::Instance().GetSystem<CharacterManager>();
            if (charMgr) {
                characterPanel_->SetCharacterList(charMgr->GetActiveCharacters());
            }
            characterPanel_->Show();
        }
    }

    void CloseCharacterPanel() {
        if (characterPanel_) characterPanel_->Hide();
    }

    void OpenFactionPanel() {
        if (factionPanel_) {
            factionPanel_->Show();
        }
    }

    void CloseFactionPanel() {
        if (factionPanel_) factionPanel_->Hide();
    }

    void FormAlliance(const std::string& factionId) {
        // TODO: Implement alliance formation logic
    }

    void OpenIntelPanel() {
        if (intelPanel_) {
            intelPanel_->Show();
        }
    }

    void CloseIntelPanel() {
        if (intelPanel_) intelPanel_->Hide();
    }

    void DeploySpy() {
        auto* intelMgr = GameSystemManager::Instance().GetSystem<IntelligenceManager>();
        if (intelMgr) {
            intelMgr->DeploySpy("Agent " + std::to_string(intelMgr->GetAllSpies().size() + 1),
                               "conservatives", "liberals", 50.0f);
        }
    }

    void MakeStoryChoice(const std::string& nodeId, const std::string& choiceId) {
        auto* storyMgr = GameSystemManager::Instance().GetSystem<StoryManager>();
        if (storyMgr) {
            storyMgr->MakeChoice(nodeId, choiceId);
        }
        UpdateHUD();
    }

    bool IsRunning() const { return running_; }

private:
    void UpdateGameLogic(float deltaTime) {
        // Update game systems
    }

    void UpdateHUD() {
        if (!gameHUD_) return;

        auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
        if (resourceMgr) {
            gameHUD_->UpdateResources(resourceMgr->GetAllResources());
        }

        const auto& gameTime = GameManager::Instance().GetGameTime();
        std::string seasonStr;
        switch (gameTime.GetSeason()) {
            case Season::Spring: seasonStr = "Spring"; break;
            case Season::Summer: seasonStr = "Summer"; break;
            case Season::Autumn: seasonStr = "Autumn"; break;
            case Season::Winter: seasonStr = "Winter"; break;
        }

        gameHUD_->UpdateTurn(
            GameManager::Instance().GetCurrentTurn(),
            gameTime.GetYear(),
            seasonStr
        );
    }

    void CheckStoryEvents() {
        auto* storyMgr = GameSystemManager::Instance().GetSystem<StoryManager>();
        if (!storyMgr) return;

        auto availableNodes = storyMgr->GetAvailableNodes();
        if (!availableNodes.empty() && storyPanel_) {
            storyPanel_->SetStoryNode(availableNodes[0]);
            storyPanel_->Show();
        }
    }

    enum class GameState {
        MainMenu,
        Playing,
        Paused
    };

    GUIRenderer* renderer_ = nullptr;
    std::unique_ptr<MainMenuScene> mainMenu_;
    std::unique_ptr<GameHUD> gameHUD_;
    std::unique_ptr<CharacterPanel> characterPanel_;
    std::unique_ptr<StoryEventPanel> storyPanel_;
    std::unique_ptr<FactionPanel> factionPanel_;
    std::unique_ptr<IntelligencePanel> intelPanel_;
    std::unique_ptr<SaveLoadPanel> saveLoadPanel_;

    GameState gameState_ = GameState::MainMenu;
    bool running_ = true;
};

GUIGame::GUIGame() = default;

bool GUIGame::Initialize() {
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

void GUIGame::Shutdown() {
    SceneManager::Instance().Shutdown();
    AudioManager::Instance().Shutdown();
    renderer_.Shutdown();
    running_ = false;
}

void GUIGame::GameLoop() {
    auto lastTime = std::chrono::high_resolution_clock::now();
    const float targetFPS = 60.0f;
    const float frameTime = 1.0f / targetFPS;

    while (running_ && SceneManager::Instance().IsRunning()) {
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

void GUIGame::ProcessEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running_ = false;
        }

        SceneManager::Instance().HandleEvent(event);
    }
}

}
