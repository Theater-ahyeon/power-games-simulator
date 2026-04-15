#include "SaveLoadPanel.h"

namespace PowerGame {

SaveLoadPanel::SaveLoadPanel(GUIRenderer* renderer, bool isSaveMode)
    : renderer_(renderer), isSaveMode_(isSaveMode) {}

SaveLoadPanel::~SaveLoadPanel() = default;

void SaveLoadPanel::Initialize() {
    CreatePanel();
    CreateSaveList();
    RefreshSaveList();
}

void SaveLoadPanel::CreatePanel() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelW = 600;
    int panelH = 500;
    int panelX = (w - panelW) / 2;
    int panelY = (h - panelH) / 2;

    panel_ = std::make_unique<GUIPanel>(panelX, panelY, panelW, panelH);
    panel_->SetBackgroundColor(Color::SurfaceLow);

    std::string title = isSaveMode_ ? "SAVE GAME" : "LOAD GAME";
    titleLabel_ = std::make_unique<GUILabel>(panelX + 20, panelY + 15, title, nullptr);
    titleLabel_->SetColor(Color::Primary);

    closeBtn_ = std::make_unique<GUIButton>(panelX + panelW - 90, panelY + 10, 70, 30, "CLOSE");
    closeBtn_->SetClickHandler([this]() {
        Hide();
        if (closeCallback_) closeCallback_();
    });

    std::string confirmText = isSaveMode_ ? "SAVE" : "LOAD";
    confirmBtn_ = std::make_unique<GUIButton>(panelX + panelW - 180, panelY + panelH - 50, 80, 35, confirmText);
    confirmBtn_->SetClickHandler([this]() {
        if (selectedSave_) {
            if (isSaveMode_ && saveCallback_) {
                saveCallback_(selectedSave_->saveName);
            } else if (!isSaveMode_ && loadCallback_) {
                loadCallback_(selectedSave_->saveId);
            }
        }
    });

    deleteBtn_ = std::make_unique<GUIButton>(panelX + panelW - 280, panelY + panelH - 50, 90, 35, "DELETE");
    deleteBtn_->SetClickHandler([this]() {
        if (selectedSave_) {
            SaveManager::Instance().DeleteSave(selectedSave_->saveId);
            RefreshSaveList();
        }
    });
}

void SaveLoadPanel::CreateSaveList() {
    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    int panelX = (w - 600) / 2;
    int panelY = (h - 500) / 2;

    saveList_ = std::make_unique<GUIListBox>(panelX + 20, panelY + 60, 300, 380);
    saveList_->SetSelectHandler([this](int index) {
        if (index >= 0 && index < static_cast<int>(saves_.size())) {
            selectedSave_ = &saves_[index];
            UpdateSaveDisplay();
        }
    });

    selectedInfoLabel_ = std::make_unique<GUILabel>(panelX + 340, panelY + 60, "Select a save file", nullptr);
    selectedInfoLabel_->SetColor(Color::TextSecondary);
}

void SaveLoadPanel::RefreshSaveList() {
    saves_ = SaveManager::Instance().GetSaveList();

    if (saveList_) {
        saveList_->ClearItems();
        for (const auto& save : saves_) {
            saveList_->AddItem(save.saveName + " - Turn " + std::to_string(save.turn));
        }
    }

    selectedSave_ = nullptr;
    UpdateSaveDisplay();
}

void SaveLoadPanel::UpdateSaveDisplay() {
    if (!selectedInfoLabel_) return;

    if (selectedSave_) {
        std::string info = "Name: " + selectedSave_->saveName + "\n";
        info += "Turn: " + std::to_string(selectedSave_->turn) + "\n";
        info += "Date: " + selectedSave_->timestamp;
        selectedInfoLabel_->SetText(info);
    } else {
        selectedInfoLabel_->SetText("Select a save file");
    }
}

void SaveLoadPanel::Render() {
    if (!visible_) return;

    int w = renderer_->GetWidth();
    int h = renderer_->GetHeight();

    renderer_->SetDrawColor(Color::FromHex(0x000000AA));
    renderer_->DrawRect(0, 0, w, h, true);

    if (panel_) panel_->Render(renderer_);
    if (titleLabel_) titleLabel_->Render(renderer_);
    if (closeBtn_) closeBtn_->Render(renderer_);
    if (confirmBtn_) confirmBtn_->Render(renderer_);
    if (deleteBtn_) deleteBtn_->Render(renderer_);

    if (saveList_) saveList_->Render(renderer_);
    if (selectedInfoLabel_) selectedInfoLabel_->Render(renderer_);
}

bool SaveLoadPanel::HandleEvent(const SDL_Event& event) {
    if (!visible_) return false;

    if (closeBtn_ && closeBtn_->HandleEvent(event)) return true;
    if (confirmBtn_ && confirmBtn_->HandleEvent(event)) return true;
    if (deleteBtn_ && deleteBtn_->HandleEvent(event)) return true;
    if (saveList_ && saveList_->HandleEvent(event)) return true;

    return false;
}

void SaveLoadPanel::Show() {
    visible_ = true;
    RefreshSaveList();
}

void SaveLoadPanel::Hide() {
    visible_ = false;
}

}
