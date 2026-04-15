#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "GUIRenderer.h"
#include "GUIComponents.h"
#include "../Story/StoryManager.h"

namespace PowerGame {

class StoryEventPanel {
public:
    StoryEventPanel(GUIRenderer* renderer);
    ~StoryEventPanel();

    void Initialize();
    void Render();
    bool HandleEvent(const SDL_Event& event);

    void SetStoryNode(StoryNode* node);

    void Show();
    void Hide();
    bool IsVisible() const { return visible_; }

    void SetChoiceCallback(std::function<void(const std::string&, const std::string&)> cb) { choiceCallback_ = cb; }

private:
    void CreatePanel();
    void CreateChoices();
    void UpdateDisplay();

    GUIRenderer* renderer_;
    bool visible_ = false;

    std::unique_ptr<GUIPanel> panel_;
    std::unique_ptr<GUIPanel> titleBar_;
    std::unique_ptr<GUILabel> titleLabel_;
    std::unique_ptr<GUILabel> typeLabel_;

    std::unique_ptr<GUIPanel> contentPanel_;
    std::unique_ptr<GUILabel> descriptionLabel_;

    std::vector<std::unique_ptr<GUIButton>> choiceButtons_;
    std::vector<std::unique_ptr<GUILabel>> choiceDescriptions_;

    StoryNode* currentNode_ = nullptr;

    std::function<void(const std::string&, const std::string&)> choiceCallback_;
};

}
