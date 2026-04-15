#include "MainMenuScene.h"
#include <cmath>

namespace PowerGame {

MainMenuScene::MainMenuScene(GUIRenderer* renderer) : renderer_(renderer) {}

MainMenuScene::~MainMenuScene() = default;

void MainMenuScene::Initialize() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    CreateTitle();
    CreateMenuButtons();
    CreateVersionInfo();
}

void MainMenuScene::CreateTitle() {
    int w = renderer_->GetWidth();

    titleLabel_ = std::make_unique<GUILabel>(
        w / 2 - 200, 80,
        "POWER GAME",
        nullptr
    );
    titleLabel_->SetColor(Color::Primary);

    subtitleLabel_ = std::make_unique<GUILabel>(
        w / 2 - 180, 130,
        "The Art of Political Intrigue",
        nullptr
    );
    subtitleLabel_->SetColor(Color::TextSecondary);
}

void MainMenuScene::CreateMenuButtons() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();
    int btnWidth = 280;
    int btnHeight = 50;
    int startY = 220;
    int spacing = 70;

    newGameBtn_ = std::make_unique<GUIButton>(
        w / 2 - btnWidth / 2, startY,
        btnWidth, btnHeight,
        "NEW GAME"
    );
    newGameBtn_->SetClickHandler([this]() {
        if (newGameCallback_) newGameCallback_();
    });

    loadGameBtn_ = std::make_unique<GUIButton>(
        w / 2 - btnWidth / 2, startY + spacing,
        btnWidth, btnHeight,
        "LOAD GAME"
    );
    loadGameBtn_->SetClickHandler([this]() {
        if (loadGameCallback_) loadGameCallback_();
    });

    settingsBtn_ = std::make_unique<GUIButton>(
        w / 2 - btnWidth / 2, startY + spacing * 2,
        btnWidth, btnHeight,
        "SETTINGS"
    );
    settingsBtn_->SetClickHandler([this]() {
        if (settingsCallback_) settingsCallback_();
    });

    quitBtn_ = std::make_unique<GUIButton>(
        w / 2 - btnWidth / 2, startY + spacing * 3,
        btnWidth, btnHeight,
        "QUIT"
    );
    quitBtn_->SetClickHandler([this]() {
        if (quitCallback_) quitCallback_();
    });
}

void MainMenuScene::CreateVersionInfo() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    versionLabel_ = std::make_unique<GUILabel>(
        w - 150, h - 30,
        "v1.0.0 - Commercial Release",
        nullptr
    );
    versionLabel_->SetColor(Color::TextSecondary);
}

void MainMenuScene::Render() {
    if (!visible_) return;

    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    renderer_->SetDrawColor(Color::Background);
    renderer_->DrawRect(0, 0, w, h, true);

    titlePulse_ += 0.02f * pulseDirection_;
    if (titlePulse_ > 1.0f) {
        titlePulse_ = 1.0f;
        pulseDirection_ = -1.0f;
    } else if (titlePulse_ < 0.0f) {
        titlePulse_ = 0.0f;
        pulseDirection_ = 1.0f;
    }

    int titleOffset = static_cast<int>(std::sin(titlePulse_ * 3.14159f) * 3);
    titleLabel_->SetPosition(w / 2 - 200, 80 + titleOffset);

    if (titleLabel_) titleLabel_->Render(renderer_);
    if (subtitleLabel_) subtitleLabel_->Render(renderer_);

    if (newGameBtn_) newGameBtn_->Render(renderer_);
    if (loadGameBtn_) loadGameBtn_->Render(renderer_);
    if (settingsBtn_) settingsBtn_->Render(renderer_);
    if (quitBtn_) quitBtn_->Render(renderer_);

    if (versionLabel_) versionLabel_->Render(renderer_);

    renderer_->SetDrawColor(Color::Outline);
    renderer_->DrawRect(20, 20, w - 40, h - 40, false);
    renderer_->DrawRect(25, 25, w - 50, h - 50, false);
}

bool MainMenuScene::HandleEvent(const SDL_Event& event) {
    if (!visible_) return false;

    if (newGameBtn_ && newGameBtn_->HandleEvent(event)) return true;
    if (loadGameBtn_ && loadGameBtn_->HandleEvent(event)) return true;
    if (settingsBtn_ && settingsBtn_->HandleEvent(event)) return true;
    if (quitBtn_ && quitBtn_->HandleEvent(event)) return true;

    return false;
}

void MainMenuScene::Show() {
    visible_ = true;
}

void MainMenuScene::Hide() {
    visible_ = false;
}

}
