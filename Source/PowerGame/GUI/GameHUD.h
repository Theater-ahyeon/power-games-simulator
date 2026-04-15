#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "GUIRenderer.h"
#include "GUIComponents.h"
#include "../Resources/ResourceManager.h"

namespace PowerGame {

class GameHUD {
public:
    GameHUD(GUIRenderer* renderer);
    ~GameHUD();

    void Initialize();
    void Render();
    bool HandleEvent(const SDL_Event& event);

    void UpdateResources(const std::unordered_map<ResourceType, float>& resources);
    void UpdateTurn(int turn, int year, const std::string& season);

    void Show();
    void Hide();
    bool IsVisible() const { return visible_; }

    void SetMenuCallback(std::function<void()> cb) { menuCallback_ = cb; }
    void SetEndTurnCallback(std::function<void()> cb) { endTurnCallback_ = cb; }
    void SetCharacterCallback(std::function<void()> cb) { characterCallback_ = cb; }
    void SetFactionCallback(std::function<void()> cb) { factionCallback_ = cb; }
    void SetIntelligenceCallback(std::function<void()> cb) { intelCallback_ = cb; }

private:
    void CreateTopBar();
    void CreateResourcePanel();
    void CreateActionButtons();
    void CreateMinimap();

    GUIRenderer* renderer_;
    bool visible_ = true;

    std::unique_ptr<GUIPanel> topBar_;
    std::unique_ptr<GUIPanel> resourcePanel_;
    std::unique_ptr<GUIPanel> actionPanel_;

    std::unique_ptr<GUILabel> turnLabel_;
    std::unique_ptr<GUILabel> dateLabel_;

    std::unordered_map<ResourceType, std::unique_ptr<GUIProgressBar>> resourceBars_;
    std::unordered_map<ResourceType, std::unique_ptr<GUILabel>> resourceLabels_;

    std::unique_ptr<GUIButton> menuBtn_;
    std::unique_ptr<GUIButton> endTurnBtn_;
    std::unique_ptr<GUIButton> charactersBtn_;
    std::unique_ptr<GUIButton> factionsBtn_;
    std::unique_ptr<GUIButton> intelBtn_;

    std::function<void()> menuCallback_;
    std::function<void()> endTurnCallback_;
    std::function<void()> characterCallback_;
    std::function<void()> factionCallback_;
    std::function<void()> intelCallback_;

    int currentTurn_ = 1;
    int currentYear_ = 1;
    std::string currentSeason_ = "Spring";
};

}
