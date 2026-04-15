#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "GUIComponents.h"

namespace PowerGame {

struct TutorialStep {
    std::string title;
    std::string description;
    std::string highlightElement;
    std::string actionHint;
    std::function<bool()> completionCondition;
    bool isCompleted = false;
};

class TutorialSystem {
public:
    static TutorialSystem& Instance();
    
    void Initialize();
    void Shutdown();
    
    void StartTutorial(const std::string& tutorialId);
    void CompleteCurrentStep();
    void SkipTutorial();
    
    bool IsTutorialActive() const { return active_; }
    TutorialStep* GetCurrentStep();
    int GetCurrentStepIndex() const { return currentStepIndex_; }
    int GetTotalSteps() const { return static_cast<int>(currentTutorial_.size()); }
    
    void RegisterTutorial(const std::string& id, const std::vector<TutorialStep>& steps);
    
    void Update();

private:
    TutorialSystem() = default;
    
    bool active_ = false;
    int currentStepIndex_ = 0;
    std::vector<TutorialStep> currentTutorial_;
    std::string currentTutorialId_;
    
    std::unordered_map<std::string, std::vector<TutorialStep>> tutorials_;
};

class TutorialUI {
public:
    TutorialUI(GUIRenderer* renderer);
    
    void Render();
    bool HandleEvent(const SDL_Event& event);
    
    void SetTutorialSystem(TutorialSystem* system) { tutorialSystem_ = system; }

private:
    GUIRenderer* renderer_;
    TutorialSystem* tutorialSystem_ = nullptr;
    
    std::unique_ptr<GUIPanel> tutorialPanel_;
    std::unique_ptr<GUILabel> titleLabel_;
    std::unique_ptr<GUILabel> descriptionLabel_;
    std::unique_ptr<GUIButton> nextButton_;
    std::unique_ptr<GUIButton> skipButton_;
    std::unique_ptr<GUIProgressBar> progressBar_;
};

}
