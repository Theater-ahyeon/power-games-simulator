#include "GameHUD.h"
#include "../Core/GameManager.h"

namespace PowerGame {

GameHUD::GameHUD(GUIRenderer* renderer) : renderer_(renderer) {}

GameHUD::~GameHUD() = default;

void GameHUD::Initialize() {
    CreateTopBar();
    CreateResourcePanel();
    CreateActionButtons();
}

void GameHUD::CreateTopBar() {
    int w = renderer_->GetWidth();

    topBar_ = std::make_unique<GUIPanel>(0, 0, w, 60);
    topBar_->SetBackgroundColor(Color::SurfaceLow);

    turnLabel_ = std::make_unique<GUILabel>(20, 20, "Turn: 1", nullptr);
    turnLabel_->SetColor(Color::Primary);

    dateLabel_ = std::make_unique<GUILabel>(w / 2 - 50, 20, "Year 1 - Spring", nullptr);
    dateLabel_->SetColor(Color::TextPrimary);

    menuBtn_ = std::make_unique<GUIButton>(w - 120, 15, 100, 30, "MENU");
    menuBtn_->SetClickHandler([this]() {
        if (menuCallback_) menuCallback_();
    });
}

void GameHUD::CreateResourcePanel() {
    int w = renderer_->GetWidth();

    resourcePanel_ = std::make_unique<GUIPanel>(w - 220, 70, 200, 350);
    resourcePanel_->SetBackgroundColor(Color::SurfaceLow);

    int y = 80;
    int barHeight = 20;
    int spacing = 35;

    auto createResourceBar = [this, &y, barHeight, spacing](ResourceType type, const std::string& name, const Color& color) {
        auto label = std::make_unique<GUILabel>(w - 210, y, name, nullptr);
        label->SetColor(Color::TextSecondary);
        resourceLabels_[type] = std::move(label);

        auto bar = std::make_unique<GUIProgressBar>(w - 210, y + 18, 180, barHeight);
        bar->SetFillColor(color);
        bar->SetProgress(0.5f);
        resourceBars_[type] = std::move(bar);

        y += spacing;
    };

    createResourceBar(ResourceType::PoliticalCapital, "Political Capital", Color::Primary);
    createResourceBar(ResourceType::Influence, "Influence", Color::Secondary);
    createResourceBar(ResourceType::Information, "Information", Color::FromHex(0x9b8bf4FF));
    createResourceBar(ResourceType::Wealth, "Wealth", Color::FromHex(0x4ade80FF));
    createResourceBar(ResourceType::Military, "Military", Color::Tertiary);
    createResourceBar(ResourceType::Reputation, "Reputation", Color::FromHex(0xf472b6FF));
    createResourceBar(ResourceType::Legitimacy, "Legitimacy", Color::FromHex(0x60a5faFF));
    createResourceBar(ResourceType::PublicSupport, "Public Support", Color::FromHex(0xfacc15FF));
}

void GameHUD::CreateActionButtons() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    actionPanel_ = std::make_unique<GUIPanel>(10, h - 70, w - 240, 60);
    actionPanel_->SetBackgroundColor(Color::SurfaceLow);

    charactersBtn_ = std::make_unique<GUIButton>(20, h - 60, 120, 40, "CHARACTERS");
    charactersBtn_->SetClickHandler([this]() {
        if (characterCallback_) characterCallback_();
    });

    factionsBtn_ = std::make_unique<GUIButton>(150, h - 60, 120, 40, "FACTIONS");
    factionsBtn_->SetClickHandler([this]() {
        if (factionCallback_) factionCallback_();
    });

    intelBtn_ = std::make_unique<GUIButton>(280, h - 60, 120, 40, "INTEL");
    intelBtn_->SetClickHandler([this]() {
        if (intelCallback_) intelCallback_();
    });

    endTurnBtn_ = std::make_unique<GUIButton>(w - 200, h - 60, 180, 40, "END TURN");
    endTurnBtn_->SetClickHandler([this]() {
        if (endTurnCallback_) endTurnCallback_();
    });
}

void GameHUD::Render() {
    if (!visible_) return;

    if (topBar_) topBar_->Render(renderer_);
    if (turnLabel_) turnLabel_->Render(renderer_);
    if (dateLabel_) dateLabel_->Render(renderer_);
    if (menuBtn_) menuBtn_->Render(renderer_);

    if (resourcePanel_) resourcePanel_->Render(renderer_);
    for (auto& [type, label] : resourceLabels_) {
        if (label) label->Render(renderer_);
    }
    for (auto& [type, bar] : resourceBars_) {
        if (bar) bar->Render(renderer_);
    }

    if (actionPanel_) actionPanel_->Render(renderer_);
    if (charactersBtn_) charactersBtn_->Render(renderer_);
    if (factionsBtn_) factionsBtn_->Render(renderer_);
    if (intelBtn_) intelBtn_->Render(renderer_);
    if (endTurnBtn_) endTurnBtn_->Render(renderer_);
}

bool GameHUD::HandleEvent(const SDL_Event& event) {
    if (!visible_) return false;

    if (menuBtn_ && menuBtn_->HandleEvent(event)) return true;
    if (endTurnBtn_ && endTurnBtn_->HandleEvent(event)) return true;
    if (charactersBtn_ && charactersBtn_->HandleEvent(event)) return true;
    if (factionsBtn_ && factionsBtn_->HandleEvent(event)) return true;
    if (intelBtn_ && intelBtn_->HandleEvent(event)) return true;

    return false;
}

void GameHUD::UpdateResources(const std::unordered_map<ResourceType, float>& resources) {
    for (const auto& [type, value] : resources) {
        auto it = resourceBars_.find(type);
        if (it != resourceBars_.end() && it->second) {
            it->second->SetProgress(value / 100.0f);
        }
    }
}

void GameHUD::UpdateTurn(int turn, int year, const std::string& season) {
    currentTurn_ = turn;
    currentYear_ = year;
    currentSeason_ = season;

    if (turnLabel_) {
        turnLabel_->SetText("Turn: " + std::to_string(turn));
    }
    if (dateLabel_) {
        dateLabel_->SetText("Year " + std::to_string(year) + " - " + season);
    }
}

void GameHUD::Show() {
    visible_ = true;
}

void GameHUD::Hide() {
    visible_ = false;
}

}
