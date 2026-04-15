#include "StoryEventPanel.h"

namespace PowerGame {

StoryEventPanel::StoryEventPanel(GUIRenderer* renderer) : renderer_(renderer) {}

StoryEventPanel::~StoryEventPanel() = default;

void StoryEventPanel::Initialize() {
    CreatePanel();
}

void StoryEventPanel::CreatePanel() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelW = 600;
    int panelH = 450;
    int panelX = (w - panelW) / 2;
    int panelY = (h - panelH) / 2;

    panel_ = std::make_unique<GUIPanel>(panelX, panelY, panelW, panelH);
    panel_->SetBackgroundColor(Color::SurfaceLow);

    titleBar_ = std::make_unique<GUIPanel>(panelX, panelY, panelW, 50);
    titleBar_->SetBackgroundColor(Color::SurfaceHigh);

    titleLabel_ = std::make_unique<GUILabel>(panelX + 20, panelY + 15, "EVENT", nullptr);
    titleLabel_->SetColor(Color::Primary);

    typeLabel_ = std::make_unique<GUILabel>(panelX + panelW - 120, panelY + 15, "", nullptr);
    typeLabel_->SetColor(Color::Secondary);

    contentPanel_ = std::make_unique<GUIPanel>(panelX + 20, panelY + 70, panelW - 40, 150);
    contentPanel_->SetBackgroundColor(Color::Surface);

    descriptionLabel_ = std::make_unique<GUILabel>(panelX + 30, panelY + 80, "", nullptr);
    descriptionLabel_->SetColor(Color::TextPrimary);
}

void StoryEventPanel::SetStoryNode(StoryNode* node) {
    currentNode_ = node;
    UpdateDisplay();
}

void StoryEventPanel::UpdateDisplay() {
    if (!currentNode_) return;

    if (titleLabel_) {
        titleLabel_->SetText(currentNode_->title);
    }

    if (typeLabel_) {
        std::string typeStr;
        switch (currentNode_->type) {
            case StoryNodeType::MainPlot: typeStr = "Main Plot"; break;
            case StoryNodeType::SideQuest: typeStr = "Side Quest"; break;
            case StoryNodeType::Crisis: typeStr = "CRISIS"; break;
            case StoryNodeType::Opportunity: typeStr = "Opportunity"; break;
            case StoryNodeType::CharacterEvent: typeStr = "Character"; break;
            case StoryNodeType::FactionEvent: typeStr = "Faction"; break;
            case StoryNodeType::Climax: typeStr = "CLIMAX"; break;
            case StoryNodeType::Ending: typeStr = "ENDING"; break;
        }
        typeLabel_->SetText(typeStr);
    }

    if (descriptionLabel_) {
        descriptionLabel_->SetText(currentNode_->description);
    }

    choiceButtons_.clear();
    choiceDescriptions_.clear();

    if (!currentNode_) return;

    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();
    int panelX = (w - 600) / 2;
    int panelY = (h - 450) / 2;

    int choiceY = panelY + 240;
    int btnWidth = 560;
    int btnHeight = 35;

    for (size_t i = 0; i < currentNode_->choices.size(); ++i) {
        const auto& choice = currentNode_->choices[i];

        auto btn = std::make_unique<GUIButton>(panelX + 20, choiceY, btnWidth, btnHeight, choice.text);
        btn->SetClickHandler([this, &choice]() {
            if (choiceCallback_ && currentNode_) {
                choiceCallback_(currentNode_->id, choice.id);
            }
            Hide();
        });
        choiceButtons_.push_back(std::move(btn));

        auto descLabel = std::make_unique<GUILabel>(panelX + 30, choiceY + 40, choice.description, nullptr);
        descLabel->SetColor(Color::TextSecondary);
        choiceDescriptions_.push_back(std::move(descLabel));

        choiceY += 70;
    }
}

void StoryEventPanel::Render() {
    if (!visible_) return;

    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    renderer_->SetDrawColor(Color::FromHex(0x000000AA));
    renderer_->DrawRect(0, 0, w, h, true);

    if (panel_) panel_->Render(renderer_);
    if (titleBar_) titleBar_->Render(renderer_);
    if (titleLabel_) titleLabel_->Render(renderer_);
    if (typeLabel_) typeLabel_->Render(renderer_);

    if (contentPanel_) contentPanel_->Render(renderer_);
    if (descriptionLabel_) descriptionLabel_->Render(renderer_);

    for (auto& btn : choiceButtons_) {
        if (btn) btn->Render(renderer_);
    }
    for (auto& desc : choiceDescriptions_) {
        if (desc) desc->Render(renderer_);
    }
}

bool StoryEventPanel::HandleEvent(const SDL_Event& event) {
    if (!visible_) return false;

    for (auto& btn : choiceButtons_) {
        if (btn && btn->HandleEvent(event)) return true;
    }

    return false;
}

void StoryEventPanel::Show() {
    visible_ = true;
}

void StoryEventPanel::Hide() {
    visible_ = false;
}

}
