#include "TutorialSystem.h"
#include "GUIRenderer.h"
#include <algorithm>

namespace PowerGame {

TutorialSystem& TutorialSystem::Instance() {
    static TutorialSystem instance;
    return instance;
}

void TutorialSystem::Initialize() {
    std::vector<TutorialStep> basicTutorial = {
        {
            "欢迎来到权力游戏",
            "这是一个关于政治阴谋和权力斗争的游戏。您的目标是通过各种策略手段，最终掌握国家权力。",
            "dashboard",
            "点击'下一步'继续",
            []() { return true; }
        },
        {
            "理解资源系统",
            "游戏中有8种核心资源：政治资本、势力影响力、情报信息、财富、军事力量、声望、合法性和民众支持。这些资源是您权力的基础。",
            "resources_panel",
            "查看右侧资源面板",
            []() { return true; }
        },
        {
            "管理派系关系",
            "游戏中有5大政治派系：保守联盟、自由联盟、中间集团、激进阵线和机会主义圈。与各派系建立良好关系是成功的关键。",
            "faction_panel",
            "查看派系面板",
            []() { return true; }
        },
        {
            "收集情报",
            "情报是权力游戏的命脉。通过部署间谍，您可以获取对手的秘密信息。",
            "intelligence_panel",
            "点击情报按钮",
            []() { return true; }
        },
        {
            "建立联盟",
            "与合适的势力建立联盟可以大大增强您的实力。但要小心，背叛的代价比结盟的收益更大。",
            "alliance_panel",
            "查看联盟面板",
            []() { return true; }
        },
        {
            "做出决策",
            "在关键剧情节点，您需要做出重要决策。每个决策都有短期和长期影响。请谨慎考虑！",
            "decision_panel",
            "等待剧情事件触发",
            []() { return true; }
        },
        {
            "教程完成",
            "恭喜您完成了基础教程！现在您可以开始您的政治生涯了。记住：在权力的游戏中，不是朋友就是敌人。",
            "start_button",
            "点击开始游戏",
            []() { return true; }
        }
    };
    
    RegisterTutorial("basic_tutorial", basicTutorial);
}

void TutorialSystem::Shutdown() {
    tutorials_.clear();
}

void TutorialSystem::StartTutorial(const std::string& tutorialId) {
    auto it = tutorials_.find(tutorialId);
    if (it != tutorials_.end()) {
        currentTutorial_ = it->second;
        currentTutorialId_ = tutorialId;
        currentStepIndex_ = 0;
        active_ = true;
    }
}

void TutorialSystem::CompleteCurrentStep() {
    if (!active_ || currentTutorial_.empty()) return;
    
    if (currentStepIndex_ < static_cast<int>(currentTutorial_.size())) {
        currentTutorial_[currentStepIndex_].isCompleted = true;
        currentStepIndex_++;
        
        if (currentStepIndex_ >= static_cast<int>(currentTutorial_.size())) {
            active_ = false;
        }
    }
}

void TutorialSystem::SkipTutorial() {
    active_ = false;
    currentTutorial_.clear();
    currentStepIndex_ = 0;
}

TutorialStep* TutorialSystem::GetCurrentStep() {
    if (!active_ || currentTutorial_.empty()) return nullptr;
    if (currentStepIndex_ >= static_cast<int>(currentTutorial_.size())) return nullptr;
    return &currentTutorial_[currentStepIndex_];
}

void TutorialSystem::RegisterTutorial(const std::string& id, const std::vector<TutorialStep>& steps) {
    tutorials_[id] = steps;
}

void TutorialSystem::Update() {
    if (!active_) return;
    
    TutorialStep* currentStep = GetCurrentStep();
    if (currentStep && currentStep->completionCondition()) {
        CompleteCurrentStep();
    }
}

TutorialUI::TutorialUI(GUIRenderer* renderer) : renderer_(renderer) {
    int centerX = renderer_->GetWidth() / 2 - 300;
    int centerY = renderer_->GetHeight() / 2 - 150;
    
    tutorialPanel_ = std::make_unique<GUIPanel>(centerX, centerY, 600, 300);
    titleLabel_ = std::make_unique<GUILabel>(centerX + 20, centerY + 20, "");
    descriptionLabel_ = std::make_unique<GUILabel>(centerX + 20, centerY + 60, "");
    progressBar_ = std::make_unique<GUIProgressBar>(centerX + 20, centerY + 250, 400, 20);
    
    nextButton_ = std::make_unique<GUIButton>(centerX + 440, centerY + 240, 140, 40, "下一步");
    nextButton_->SetClickHandler([]() {
        TutorialSystem::Instance().CompleteCurrentStep();
    });
    
    skipButton_ = std::make_unique<GUIButton>(centerX + 20, centerY + 240, 100, 40, "跳过");
    skipButton_->SetClickHandler([]() {
        TutorialSystem::Instance().SkipTutorial();
    });
}

void TutorialUI::Render() {
    if (!tutorialSystem_ || !tutorialSystem_->IsTutorialActive()) return;
    
    TutorialStep* step = tutorialSystem_->GetCurrentStep();
    if (!step) return;
    
    titleLabel_->SetText(step->title);
    descriptionLabel_->SetText(step->description);
    
    float progress = static_cast<float>(tutorialSystem_->GetCurrentStepIndex()) / 
                    static_cast<float>(tutorialSystem_->GetTotalSteps());
    progressBar_->SetProgress(progress);
    
    tutorialPanel_->Render(renderer_);
    titleLabel_->Render(renderer_);
    descriptionLabel_->Render(renderer_);
    progressBar_->Render(renderer_);
    nextButton_->Render(renderer_);
    skipButton_->Render(renderer_);
}

bool TutorialUI::HandleEvent(const SDL_Event& event) {
    if (!tutorialSystem_ || !tutorialSystem_->IsTutorialActive()) return false;
    
    nextButton_->HandleEvent(event);
    skipButton_->HandleEvent(event);
    
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN) {
            TutorialSystem::Instance().CompleteCurrentStep();
            return true;
        } else if (event.key.keysym.sym == SDLK_ESCAPE) {
            TutorialSystem::Instance().SkipTutorial();
            return true;
        }
    }
    
    return false;
}

}
