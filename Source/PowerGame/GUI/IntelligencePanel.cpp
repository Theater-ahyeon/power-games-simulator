#include "IntelligencePanel.h"

namespace PowerGame {

IntelligencePanel::IntelligencePanel(GUIRenderer* renderer) : renderer_(renderer) {}

IntelligencePanel::~IntelligencePanel() = default;

void IntelligencePanel::Initialize() {
    CreatePanel();
    CreateSpyList();
    CreateIntelList();
    CreateActions();
}

void IntelligencePanel::CreatePanel() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelW = 800;
    int panelH = 600;
    int panelX = (w - panelW) / 2;
    int panelY = (h - panelH) / 2;

    panel_ = std::make_unique<GUIPanel>(panelX, panelY, panelW, panelH);
    panel_->SetBackgroundColor(Color::SurfaceLow);

    titleLabel_ = std::make_unique<GUILabel>(panelX + 20, panelY + 15, "INTELLIGENCE", nullptr);
    titleLabel_->SetColor(Color::Primary);

    closeBtn_ = std::make_unique<GUIButton>(panelX + panelW - 90, panelY + 10, 70, 30, "CLOSE");
    closeBtn_->SetClickHandler([this]() {
        Hide();
        if (closeCallback_) closeCallback_();
    });
}

void IntelligencePanel::CreateSpyList() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelX = (w - 800) / 2;
    int panelY = (h - 600) / 2;

    spySectionLabel_ = std::make_unique<GUILabel>(panelX + 20, panelY + 55, "Active Agents", nullptr);
    spySectionLabel_->SetColor(Color::Secondary);

    spyList_ = std::make_unique<GUIListBox>(panelX + 20, panelY + 80, 200, 200);
    spyList_->SetSelectHandler([this](int index) {
        if (index >= 0 && index < static_cast<int>(spies_.size())) {
            selectedSpy_ = &spies_[index];
            UpdateDisplay();
        }
    });

    spyDetailLabel_ = std::make_unique<GUILabel>(panelX + 240, panelY + 80, "Select an agent", nullptr);
    spyDetailLabel_->SetColor(Color::TextSecondary);
}

void IntelligencePanel::CreateIntelList() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelX = (w - 800) / 2;
    int panelY = (h - 600) / 2;

    intelSectionLabel_ = std::make_unique<GUILabel>(panelX + 20, panelY + 300, "Intelligence Database", nullptr);
    intelSectionLabel_->SetColor(Color::Secondary);

    intelList_ = std::make_unique<GUIListBox>(panelX + 20, panelY + 325, 350, 200);
    intelList_->SetSelectHandler([this](int index) {
        if (index >= 0 && index < static_cast<int>(intelPieces_.size())) {
            selectedIntel_ = &intelPieces_[index];
            UpdateDisplay();
        }
    });

    intelDetailLabel_ = std::make_unique<GUILabel>(panelX + 390, panelY + 325, "Select intelligence", nullptr);
    intelDetailLabel_->SetColor(Color::TextSecondary);
}

void IntelligencePanel::CreateActions() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelX = (w - 800) / 2;
    int panelY = (h - 600) / 2;

    deploySpyBtn_ = std::make_unique<GUIButton>(panelX + 600, panelY + 80, 120, 35, "DEPLOY SPY");
    deploySpyBtn_->SetClickHandler([this]() {
        if (deploySpyCallback_) deploySpyCallback_();
        RefreshData();
    });

    recallSpyBtn_ = std::make_unique<GUIButton>(panelX + 600, panelY + 125, 120, 35, "RECALL SPY");

    analyzeBtn_ = std::make_unique<GUIButton>(panelX + 600, panelY + 325, 120, 35, "ANALYZE");
    analyzeBtn_->SetClickHandler([this]() {
        if (selectedIntel_ && analyzeCallback_) {
            analyzeCallback_(selectedIntel_->id);
        }
    });

    sweepBtn_ = std::make_unique<GUIButton>(panelX + 600, panelY + 370, 120, 35, "SWEEP");
}

void IntelligencePanel::RefreshData() {
    spies_.clear();
    intelPieces_.clear();

    auto* intelMgr = GameSystemManager::Instance().GetSystem<IntelligenceManager>();
    if (intelMgr) {
        for (const auto& [id, spy] : intelMgr->GetAllSpies()) {
            spies_.push_back(spy);
        }

        // Get all intel pieces
        // Note: This would need to be implemented in IntelligenceManager
    }

    if (spyList_) {
        spyList_->ClearItems();
        for (const auto& spy : spies_) {
            spyList_->AddItem(spy.name + " - " + spy.targetFactionId);
        }
    }

    if (intelList_) {
        intelList_->ClearItems();
        for (const auto& intel : intelPieces_) {
            intelList_->AddItem(GetIntelTypeString(intel.type) + " - " + intel.targetId);
        }
    }

    selectedSpy_ = nullptr;
    selectedIntel_ = nullptr;
    UpdateDisplay();
}

void IntelligencePanel::UpdateDisplay() {
    if (selectedSpy_ && spyDetailLabel_) {
        std::string info = "Name: " + selectedSpy_->name + "\n";
        info += "Target: " + selectedSpy_->targetFactionId + "\n";
        info += "Skill: " + std::to_string(static_cast<int>(selectedSpy_->skill)) + "\n";
        info += "Loyalty: " + std::to_string(static_cast<int>(selectedSpy_->loyalty)) + "\n";
        info += "Risk: " + std::to_string(static_cast<int>(selectedSpy_->exposureRisk)) + "%";
        spyDetailLabel_->SetText(info);
    } else if (spyDetailLabel_) {
        spyDetailLabel_->SetText("Select an agent");
    }

    if (selectedIntel_ && intelDetailLabel_) {
        std::string info = "Type: " + GetIntelTypeString(selectedIntel_->type) + "\n";
        info += "Target: " + selectedIntel_->targetId + "\n";
        info += "Reliability: " + GetReliabilityString(selectedIntel_->reliability) + "\n";
        info += "Value: " + std::to_string(static_cast<int>(selectedIntel_->value));
        intelDetailLabel_->SetText(info);
    } else if (intelDetailLabel_) {
        intelDetailLabel_->SetText("Select intelligence");
    }
}

std::string IntelligencePanel::GetIntelTypeString(IntelType type) const {
    switch (type) {
        case IntelType::PoliticalSecret: return "Political Secret";
        case IntelType::FinancialRecord: return "Financial Record";
        case IntelType::MilitaryIntel: return "Military Intel";
        case IntelType::PersonalScandal: return "Personal Scandal";
        case IntelType::AllianceDetail: return "Alliance Detail";
        case IntelType::BetrayalPlan: return "Betrayal Plan";
        case IntelType::PolicyLeak: return "Policy Leak";
        case IntelType::ForeignConnection: return "Foreign Connection";
        default: return "Unknown";
    }
}

std::string IntelligencePanel::GetReliabilityString(IntelReliability reliability) const {
    switch (reliability) {
        case IntelReliability::Unverified: return "Unverified";
        case IntelReliability::Low: return "Low";
        case IntelReliability::Medium: return "Medium";
        case IntelReliability::High: return "High";
        case IntelReliability::Confirmed: return "Confirmed";
        default: return "Unknown";
    }
}

void IntelligencePanel::Render() {
    if (!visible_) return;

    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    renderer_->SetDrawColor(Color::FromHex(0x000000AA));
    renderer_->DrawRect(0, 0, w, h, true);

    if (panel_) panel_->Render(renderer_);
    if (titleLabel_) titleLabel_->Render(renderer_);
    if (closeBtn_) closeBtn_->Render(renderer_);

    if (spySectionLabel_) spySectionLabel_->Render(renderer_);
    if (spyList_) spyList_->Render(renderer_);
    if (spyDetailLabel_) spyDetailLabel_->Render(renderer_);

    if (intelSectionLabel_) intelSectionLabel_->Render(renderer_);
    if (intelList_) intelList_->Render(renderer_);
    if (intelDetailLabel_) intelDetailLabel_->Render(renderer_);

    if (deploySpyBtn_) deploySpyBtn_->Render(renderer_);
    if (recallSpyBtn_) recallSpyBtn_->Render(renderer_);
    if (analyzeBtn_) analyzeBtn_->Render(renderer_);
    if (sweepBtn_) sweepBtn_->Render(renderer_);
}

bool IntelligencePanel::HandleEvent(const SDL_Event& event) {
    if (!visible_) return false;

    if (closeBtn_ && closeBtn_->HandleEvent(event)) return true;
    if (spyList_ && spyList_->HandleEvent(event)) return true;
    if (intelList_ && intelList_->HandleEvent(event)) return true;
    if (deploySpyBtn_ && deploySpyBtn_->HandleEvent(event)) return true;
    if (recallSpyBtn_ && recallSpyBtn_->HandleEvent(event)) return true;
    if (analyzeBtn_ && analyzeBtn_->HandleEvent(event)) return true;
    if (sweepBtn_ && sweepBtn_->HandleEvent(event)) return true;

    return false;
}

void IntelligencePanel::Show() {
    visible_ = true;
    RefreshData();
}

void IntelligencePanel::Hide() {
    visible_ = false;
}

}
