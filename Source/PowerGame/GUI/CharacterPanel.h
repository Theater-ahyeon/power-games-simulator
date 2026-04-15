#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "GUIRenderer.h"
#include "GUIComponents.h"
#include "../Character/CharacterManager.h"

namespace PowerGame {

class CharacterPanel {
public:
    CharacterPanel(GUIRenderer* renderer);
    ~CharacterPanel();

    void Initialize();
    void Render();
    bool HandleEvent(const SDL_Event& event);

    void SetCharacterList(const std::vector<Character*>& characters);
    void SelectCharacter(const std::string& characterId);

    void Show();
    void Hide();
    bool IsVisible() const { return visible_; }

    void SetCloseCallback(std::function<void()> cb) { closeCallback_ = cb; }
    void SetRecruitCallback(std::function<void(const std::string&)> cb) { recruitCallback_ = cb; }
    void SetBetrayCallback(std::function<void(const std::string&)> cb) { betrayCallback_ = cb; }

private:
    void CreatePanel();
    void CreateCharacterList();
    void CreateCharacterDetails();
    void UpdateCharacterDisplay();

    GUIRenderer* renderer_;
    bool visible_ = false;

    std::unique_ptr<GUIPanel> panel_;
    std::unique_ptr<GUILabel> titleLabel_;
    std::unique_ptr<GUIButton> closeBtn_;

    std::unique_ptr<GUIListBox> characterList_;
    std::unique_ptr<GUILabel> selectedNameLabel_;
    std::unique_ptr<GUILabel> selectedFactionLabel_;
    std::unique_ptr<GUILabel> selectedStanceLabel_;

    std::vector<std::unique_ptr<GUIProgressBar>> attributeBars_;
    std::vector<std::unique_ptr<GUILabel>> attributeLabels_;

    std::unique_ptr<GUIButton> recruitBtn_;
    std::unique_ptr<GUIButton> betrayBtn_;
    std::unique_ptr<GUIButton> intelBtn_;

    std::vector<Character*> characters_;
    Character* selectedCharacter_ = nullptr;

    std::function<void()> closeCallback_;
    std::function<void(const std::string&)> recruitCallback_;
    std::function<void(const std::string&)> betrayCallback_;
};

}
