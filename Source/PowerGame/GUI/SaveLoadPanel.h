#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "GUIRenderer.h"
#include "GUIComponents.h"
#include "../Data/SaveManager.h"

namespace PowerGame {

class SaveLoadPanel {
public:
    SaveLoadPanel(GUIRenderer* renderer, bool isSaveMode = true);
    ~SaveLoadPanel();

    void Initialize();
    void Render();
    bool HandleEvent(const SDL_Event& event);

    void RefreshSaveList();

    void Show();
    void Hide();
    bool IsVisible() const { return visible_; }

    void SetCloseCallback(std::function<void()> cb) { closeCallback_ = cb; }
    void SetSaveCallback(std::function<void(const std::string&)> cb) { saveCallback_ = cb; }
    void SetLoadCallback(std::function<void(const std::string&)> cb) { loadCallback_ = cb; }

private:
    void CreatePanel();
    void CreateSaveList();
    void CreateInputField();
    void UpdateSaveDisplay();

    GUIRenderer* renderer_;
    bool visible_ = false;
    bool isSaveMode_;

    std::unique_ptr<GUIPanel> panel_;
    std::unique_ptr<GUILabel> titleLabel_;
    std::unique_ptr<GUIButton> closeBtn_;
    std::unique_ptr<GUIButton> confirmBtn_;
    std::unique_ptr<GUIButton> deleteBtn_;

    std::unique_ptr<GUIListBox> saveList_;
    std::unique_ptr<GUILabel> selectedInfoLabel_;

    std::vector<SaveData> saves_;
    SaveData* selectedSave_ = nullptr;

    std::function<void()> closeCallback_;
    std::function<void(const std::string&)> saveCallback_;
    std::function<void(const std::string&)> loadCallback_;
};

}
