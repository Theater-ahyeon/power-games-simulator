#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include <functional>
#include "GUIRenderer.h"
#include "GUIComponents.h"

namespace PowerGame {

class MainMenuScene {
public:
    using MenuCallback = std::function<void()>;

    MainMenuScene(GUIRenderer* renderer);
    ~MainMenuScene();

    void Initialize();
    void Render();
    bool HandleEvent(const SDL_Event& event);

    void SetNewGameCallback(MenuCallback cb) { newGameCallback_ = cb; }
    void SetLoadGameCallback(MenuCallback cb) { loadGameCallback_ = cb; }
    void SetSettingsCallback(MenuCallback cb) { settingsCallback_ = cb; }
    void SetQuitCallback(MenuCallback cb) { quitCallback_ = cb; }

    void Show();
    void Hide();
    bool IsVisible() const { return visible_; }

private:
    void CreateTitle();
    void CreateMenuButtons();
    void CreateBackground();
    void CreateVersionInfo();

    GUIRenderer* renderer_;
    bool visible_ = true;

    std::unique_ptr<GUILabel> titleLabel_;
    std::unique_ptr<GUILabel> subtitleLabel_;
    std::unique_ptr<GUILabel> versionLabel_;

    std::unique_ptr<GUIButton> newGameBtn_;
    std::unique_ptr<GUIButton> loadGameBtn_;
    std::unique_ptr<GUIButton> settingsBtn_;
    std::unique_ptr<GUIButton> quitBtn_;

    MenuCallback newGameCallback_;
    MenuCallback loadGameCallback_;
    MenuCallback settingsCallback_;
    MenuCallback quitCallback_;

    float titlePulse_ = 0.0f;
    float pulseDirection_ = 1.0f;
};

}
