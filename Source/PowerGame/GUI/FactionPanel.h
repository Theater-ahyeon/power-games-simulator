#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "GUIRenderer.h"
#include "GUIComponents.h"
#include "../Faction/FactionManager.h"

namespace PowerGame {

class FactionPanel {
public:
    FactionPanel(GUIRenderer* renderer);
    ~FactionPanel();

    void Initialize();
    void Render();
    bool HandleEvent(const SDL_Event& event);

    void RefreshFactions();

    void Show();
    void Hide();
    bool IsVisible() const { return visible_; }

    void SetCloseCallback(std::function<void()> cb) { closeCallback_ = cb; }
    void SetFormAllianceCallback(std::function<void(const std::string&)> cb) { allianceCallback_ = cb; }
    void SetBetrayCallback(std::function<void(const std::string&)> cb) { betrayCallback_ = cb; }

private:
    void CreatePanel();
    void CreateFactionList();
    void CreateFactionDetails();
    void UpdateFactionDisplay();
    Color GetIdeologyColor(FactionIdeology ideology) const;

    GUIRenderer* renderer_;
    bool visible_ = false;

    std::unique_ptr<GUIPanel> panel_;
    std::unique_ptr<GUILabel> titleLabel_;
    std::unique_ptr<GUIButton> closeBtn_;

    std::unique_ptr<GUIListBox> factionList_;
    std::unique_ptr<GUILabel> selectedNameLabel_;
    std::unique_ptr<GUILabel> selectedIdeologyLabel_;
    std::unique_ptr<GUILabel> memberCountLabel_;

    std::unique_ptr<GUIProgressBar> powerBar_;
    std::unique_ptr<GUIProgressBar> cohesionBar_;
    std::unique_ptr<GUIProgressBar> influenceBar_;

    std::unique_ptr<GUIButton> allianceBtn_;
    std::unique_ptr<GUIButton> betrayBtn_;
    std::unique_ptr<GUIButton> intelBtn_;

    std::vector<Faction*> factions_;
    Faction* selectedFaction_ = nullptr;

    std::function<void()> closeCallback_;
    std::function<void(const std::string&)> allianceCallback_;
    std::function<void(const std::string&)> betrayCallback_;
};

}
