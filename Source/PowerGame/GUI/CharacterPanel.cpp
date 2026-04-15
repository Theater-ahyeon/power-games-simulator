#include "CharacterPanel.h"

namespace PowerGame {

CharacterPanel::CharacterPanel(GUIRenderer* renderer) : renderer_(renderer) {}

CharacterPanel::~CharacterPanel() = default;

void CharacterPanel::Initialize() {
    CreatePanel();
    CreateCharacterList();
    CreateCharacterDetails();
}

void CharacterPanel::CreatePanel() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelW = 700;
    int panelH = 500;
    int panelX = (w - panelW) / 2;
    int panelY = (h - panelH) / 2;

    panel_ = std::make_unique<GUIPanel>(panelX, panelY, panelW, panelH);
    panel_->SetBackgroundColor(Color::SurfaceLow);

    titleLabel_ = std::make_unique<GUILabel>(panelX + 20, panelY + 15, "CHARACTERS", nullptr);
    titleLabel_->SetColor(Color::Primary);

    closeBtn_ = std::make_unique<GUIButton>(panelX + panelW - 90, panelY + 10, 70, 30, "CLOSE");
    closeBtn_->SetClickHandler([this]() {
        Hide();
        if (closeCallback_) closeCallback_();
    });
}

void CharacterPanel::CreateCharacterList() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelX = (w - 700) / 2;
    int panelY = (h - 500) / 2;

    characterList_ = std::make_unique<GUIListBox>(panelX + 20, panelY + 60, 200, 400);
    characterList_->SetSelectHandler([this](int index) {
        if (index >= 0 && index < static_cast<int>(characters_.size())) {
            SelectCharacter(characters_[index]->GetId());
        }
    });
}

void CharacterPanel::CreateCharacterDetails() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelX = (w - 700) / 2;
    int panelY = (h - 500) / 2;

    int detailX = panelX + 240;
    int detailY = panelY + 60;

    selectedNameLabel_ = std::make_unique<GUILabel>(detailX, detailY, "Select a character", nullptr);
    selectedNameLabel_->SetColor(Color::TextPrimary);

    selectedFactionLabel_ = std::make_unique<GUILabel>(detailX, detailY + 25, "", nullptr);
    selectedFactionLabel_->SetColor(Color::TextSecondary);

    selectedStanceLabel_ = std::make_unique<GUILabel>(detailX, detailY + 50, "", nullptr);
    selectedStanceLabel_->SetColor(Color::Secondary);

    std::vector<std::pair<std::string, Color>> attributes = {
        {"Scheming", Color::FromHex(0x9b8bf4FF)},
        {"Charisma", Color::Primary},
        {"Intelligence", Color::FromHex(0x4ade80FF)},
        {"Wealth", Color::Secondary},
        {"Military", Color::Tertiary},
        {"Reputation", Color::FromHex(0xf472b6FF)},
        {"Integrity", Color::FromHex(0x60a5faFF)},
        {"Ambition", Color::FromHex(0xfacc15FF)}
    };

    int attrY = detailY + 90;
    for (const auto& [name, color] : attributes) {
        auto label = std::make_unique<GUILabel>(detailX, attrY, name, nullptr);
        label->SetColor(Color::TextSecondary);
        attributeLabels_.push_back(std::move(label));

        auto bar = std::make_unique<GUIProgressBar>(detailX + 100, attrY, 150, 16);
        bar->SetFillColor(color);
        bar->SetProgress(0.0f);
        attributeBars_.push_back(std::move(bar));

        attrY += 28;
    }

    recruitBtn_ = std::make_unique<GUIButton>(detailX, panelY + 400, 100, 35, "RECRUIT");
    recruitBtn_->SetClickHandler([this]() {
        if (selectedCharacter_ && recruitCallback_) {
            recruitCallback_(selectedCharacter_->GetId());
        }
    });

    betrayBtn_ = std::make_unique<GUIButton>(detailX + 110, panelY + 400, 100, 35, "BETRAY");
    betrayBtn_->SetClickHandler([this]() {
        if (selectedCharacter_ && betrayCallback_) {
            betrayCallback_(selectedCharacter_->GetId());
        }
    });

    intelBtn_ = std::make_unique<GUIButton>(detailX + 220, panelY + 400, 100, 35, "INTEL");
}

void CharacterPanel::SetCharacterList(const std::vector<Character*>& characters) {
    characters_ = characters;
    if (characterList_) {
        characterList_->ClearItems();
        for (auto* character : characters) {
            if (character) {
                characterList_->AddItem(character->GetName());
            }
        }
    }
}

void CharacterPanel::SelectCharacter(const std::string& characterId) {
    for (auto* character : characters_) {
        if (character && character->GetId() == characterId) {
            selectedCharacter_ = character;
            UpdateCharacterDisplay();
            break;
        }
    }
}

void CharacterPanel::UpdateCharacterDisplay() {
    if (!selectedCharacter_) return;

    if (selectedNameLabel_) {
        selectedNameLabel_->SetText(selectedCharacter_->GetName());
    }

    if (selectedFactionLabel_) {
        selectedFactionLabel_->SetText("Faction: " + selectedCharacter_->GetFactionId());
    }

    if (selectedStanceLabel_) {
        std::string stanceStr;
        switch (selectedCharacter_->GetStance()) {
            case PoliticalStance::Conservative: stanceStr = "Conservative"; break;
            case PoliticalStance::Liberal: stanceStr = "Liberal"; break;
            case PoliticalStance::Neutral: stanceStr = "Neutral"; break;
            case PoliticalStance::Radical: stanceStr = "Radical"; break;
            case PoliticalStance::Opportunist: stanceStr = "Opportunist"; break;
        }
        selectedStanceLabel_->SetText("Stance: " + stanceStr);
    }

    const auto& attrs = selectedCharacter_->GetAttributes();
    std::vector<float> values = {
        attrs.scheming, attrs.charisma, attrs.intelligence, attrs.wealth,
        attrs.military, attrs.reputation, attrs.integrity, attrs.ambition
    };

    for (size_t i = 0; i < attributeBars_.size() && i < values.size(); ++i) {
        if (attributeBars_[i]) {
            attributeBars_[i]->SetProgress(values[i] / 100.0f);
        }
    }
}

void CharacterPanel::Render() {
    if (!visible_) return;

    if (panel_) panel_->Render(renderer_);
    if (titleLabel_) titleLabel_->Render(renderer_);
    if (closeBtn_) closeBtn_->Render(renderer_);

    if (characterList_) characterList_->Render(renderer_);

    if (selectedNameLabel_) selectedNameLabel_->Render(renderer_);
    if (selectedFactionLabel_) selectedFactionLabel_->Render(renderer_);
    if (selectedStanceLabel_) selectedStanceLabel_->Render(renderer_);

    for (auto& label : attributeLabels_) {
        if (label) label->Render(renderer_);
    }
    for (auto& bar : attributeBars_) {
        if (bar) bar->Render(renderer_);
    }

    if (recruitBtn_) recruitBtn_->Render(renderer_);
    if (betrayBtn_) betrayBtn_->Render(renderer_);
    if (intelBtn_) intelBtn_->Render(renderer_);
}

bool CharacterPanel::HandleEvent(const SDL_Event& event) {
    if (!visible_) return false;

    if (closeBtn_ && closeBtn_->HandleEvent(event)) return true;
    if (characterList_ && characterList_->HandleEvent(event)) return true;
    if (recruitBtn_ && recruitBtn_->HandleEvent(event)) return true;
    if (betrayBtn_ && betrayBtn_->HandleEvent(event)) return true;
    if (intelBtn_ && intelBtn_->HandleEvent(event)) return true;

    return false;
}

void CharacterPanel::Show() {
    visible_ = true;
}

void CharacterPanel::Hide() {
    visible_ = false;
}

}
