#include "FactionPanel.h"

namespace PowerGame {

FactionPanel::FactionPanel(GUIRenderer* renderer) : renderer_(renderer) {}

FactionPanel::~FactionPanel() = default;

void FactionPanel::Initialize() {
    CreatePanel();
    CreateFactionList();
    CreateFactionDetails();
}

void FactionPanel::CreatePanel() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelW = 700;
    int panelH = 500;
    int panelX = (w - panelW) / 2;
    int panelY = (h - panelH) / 2;

    panel_ = std::make_unique<GUIPanel>(panelX, panelY, panelW, panelH);
    panel_->SetBackgroundColor(Color::SurfaceLow);

    titleLabel_ = std::make_unique<GUILabel>(panelX + 20, panelY + 15, "FACTIONS", nullptr);
    titleLabel_->SetColor(Color::Primary);

    closeBtn_ = std::make_unique<GUIButton>(panelX + panelW - 90, panelY + 10, 70, 30, "CLOSE");
    closeBtn_->SetClickHandler([this]() {
        Hide();
        if (closeCallback_) closeCallback_();
    });
}

void FactionPanel::CreateFactionList() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelX = (w - 700) / 2;
    int panelY = (h - 500) / 2;

    factionList_ = std::make_unique<GUIListBox>(panelX + 20, panelY + 60, 250, 400);
    factionList_->SetSelectHandler([this](int index) {
        if (index >= 0 && index < static_cast<int>(factions_.size())) {
            selectedFaction_ = factions_[index];
            UpdateFactionDisplay();
        }
    });
}

void FactionPanel::CreateFactionDetails() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelX = (w - 700) / 2;
    int panelY = (h - 500) / 2;

    int detailX = panelX + 290;
    int detailY = panelY + 60;

    selectedNameLabel_ = std::make_unique<GUILabel>(detailX, detailY, "Select a faction", nullptr);
    selectedNameLabel_->SetColor(Color::TextPrimary);

    selectedIdeologyLabel_ = std::make_unique<GUILabel>(detailX, detailY + 25, "", nullptr);
    selectedIdeologyLabel_->SetColor(Color::TextSecondary);

    memberCountLabel_ = std::make_unique<GUILabel>(detailX, detailY + 50, "", nullptr);
    memberCountLabel_->SetColor(Color::TextSecondary);

    int barY = detailY + 90;
    auto powerLabel = std::make_unique<GUILabel>(detailX, barY, "Power", nullptr);
    powerLabel->SetColor(Color::TextSecondary);
    powerBar_ = std::make_unique<GUIProgressBar>(detailX + 80, barY, 200, 18);
    powerBar_->SetFillColor(Color::Primary);

    barY += 35;
    auto cohesionLabel = std::make_unique<GUILabel>(detailX, barY, "Cohesion", nullptr);
    cohesionLabel->SetColor(Color::TextSecondary);
    cohesionBar_ = std::make_unique<GUIProgressBar>(detailX + 80, barY, 200, 18);
    cohesionBar_->SetFillColor(Color::Secondary);

    barY += 35;
    auto influenceLabel = std::make_unique<GUILabel>(detailX, barY, "Influence", nullptr);
    influenceLabel->SetColor(Color::TextSecondary);
    influenceBar_ = std::make_unique<GUIProgressBar>(detailX + 80, barY, 200, 18);
    influenceBar_->SetFillColor(Color::FromHex(0x9b8bf4FF));

    allianceBtn_ = std::make_unique<GUIButton>(detailX, panelY + 400, 100, 35, "ALLIANCE");
    allianceBtn_->SetClickHandler([this]() {
        if (selectedFaction_ && allianceCallback_) {
            allianceCallback_(selectedFaction_->GetId());
        }
    });

    betrayBtn_ = std::make_unique<GUIButton>(detailX + 110, panelY + 400, 100, 35, "BETRAY");
    betrayBtn_->SetClickHandler([this]() {
        if (selectedFaction_ && betrayCallback_) {
            betrayCallback_(selectedFaction_->GetId());
        }
    });

    intelBtn_ = std::make_unique<GUIButton>(detailX + 220, panelY + 400, 100, 35, "INTEL");
}

void FactionPanel::RefreshFactions() {
    factions_.clear();

    auto* factionMgr = GameSystemManager::Instance().GetSystem<FactionManager>();
    if (factionMgr) {
        for (const auto& [id, faction] : factionMgr->GetAllFactions()) {
            if (faction && faction->IsActive()) {
                factions_.push_back(faction.get());
            }
        }
    }

    if (factionList_) {
        factionList_->ClearItems();
        for (auto* faction : factions_) {
            if (faction) {
                factionList_->AddItem(faction->GetName());
            }
        }
    }

    selectedFaction_ = nullptr;
    UpdateFactionDisplay();
}

void FactionPanel::UpdateFactionDisplay() {
    if (!selectedFaction_) {
        if (selectedNameLabel_) selectedNameLabel_->SetText("Select a faction");
        if (selectedIdeologyLabel_) selectedIdeologyLabel_->SetText("");
        if (memberCountLabel_) memberCountLabel_->SetText("");
        if (powerBar_) powerBar_->SetProgress(0);
        if (cohesionBar_) cohesionBar_->SetProgress(0);
        if (influenceBar_) influenceBar_->SetProgress(0);
        return;
    }

    if (selectedNameLabel_) {
        selectedNameLabel_->SetText(selectedFaction_->GetName());
    }

    if (selectedIdeologyLabel_) {
        std::string ideologyStr;
        switch (selectedFaction_->GetIdeology()) {
            case FactionIdeology::Conservative: ideologyStr = "Conservative"; break;
            case FactionIdeology::Liberal: ideologyStr = "Liberal"; break;
            case FactionIdeology::Neutral: ideologyStr = "Neutral"; break;
            case FactionIdeology::Radical: ideologyStr = "Radical"; break;
            case FactionIdeology::Opportunist: ideologyStr = "Opportunist"; break;
        }
        selectedIdeologyLabel_->SetText("Ideology: " + ideologyStr);
    }

    if (memberCountLabel_) {
        memberCountLabel_->SetText("Members: " + std::to_string(selectedFaction_->GetMemberCount()));
    }

    if (powerBar_) powerBar_->SetProgress(selectedFaction_->GetPower() / 100.0f);
    if (cohesionBar_) cohesionBar_->SetProgress(selectedFaction_->GetCohesion() / 100.0f);
    if (influenceBar_) influenceBar_->SetProgress(selectedFaction_->GetInfluence() / 100.0f);
}

Color FactionPanel::GetIdeologyColor(FactionIdeology ideology) const {
    switch (ideology) {
        case FactionIdeology::Conservative: return Color::FromHex(0x4ade80FF);
        case FactionIdeology::Liberal: return Color::FromHex(0x60a5faFF);
        case FactionIdeology::Neutral: return Color::FromHex(0xe9c349FF);
        case FactionIdeology::Radical: return Color::FromHex(0xffb4acFF);
        case FactionIdeology::Opportunist: return Color::FromHex(0x9b8bf4FF);
        default: return Color::TextSecondary;
    }
}

void FactionPanel::Render() {
    if (!visible_) return;

    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    renderer_->SetDrawColor(Color::FromHex(0x000000AA));
    renderer_->DrawRect(0, 0, w, h, true);

    if (panel_) panel_->Render(renderer_);
    if (titleLabel_) titleLabel_->Render(renderer_);
    if (closeBtn_) closeBtn_->Render(renderer_);

    if (factionList_) factionList_->Render(renderer_);

    if (selectedNameLabel_) selectedNameLabel_->Render(renderer_);
    if (selectedIdeologyLabel_) selectedIdeologyLabel_->Render(renderer_);
    if (memberCountLabel_) memberCountLabel_->Render(renderer_);

    if (powerBar_) powerBar_->Render(renderer_);
    if (cohesionBar_) cohesionBar_->Render(renderer_);
    if (influenceBar_) influenceBar_->Render(renderer_);

    if (allianceBtn_) allianceBtn_->Render(renderer_);
    if (betrayBtn_) betrayBtn_->Render(renderer_);
    if (intelBtn_) intelBtn_->Render(renderer_);
}

bool FactionPanel::HandleEvent(const SDL_Event& event) {
    if (!visible_) return false;

    if (closeBtn_ && closeBtn_->HandleEvent(event)) return true;
    if (factionList_ && factionList_->HandleEvent(event)) return true;
    if (allianceBtn_ && allianceBtn_->HandleEvent(event)) return true;
    if (betrayBtn_ && betrayBtn_->HandleEvent(event)) return true;
    if (intelBtn_ && intelBtn_->HandleEvent(event)) return true;

    return false;
}

void FactionPanel::Show() {
    visible_ = true;
    RefreshFactions();
}

void FactionPanel::Hide() {
    visible_ = false;
}

}
