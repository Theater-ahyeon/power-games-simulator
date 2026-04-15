#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "GUIRenderer.h"
#include "GUIComponents.h"
#include "../Intelligence/IntelligenceManager.h"

namespace PowerGame {

class IntelligencePanel {
public:
    IntelligencePanel(GUIRenderer* renderer);
    ~IntelligencePanel();

    void Initialize();
    void Render();
    bool HandleEvent(const SDL_Event& event);

    void RefreshData();

    void Show();
    void Hide();
    bool IsVisible() const { return visible_; }

    void SetCloseCallback(std::function<void()> cb) { closeCallback_ = cb; }
    void SetDeploySpyCallback(std::function<void()> cb) { deploySpyCallback_ = cb; }
    void SetAnalyzeIntelCallback(std::function<void(const std::string&)> cb) { analyzeCallback_ = cb; }

private:
    void CreatePanel();
    void CreateSpyList();
    void CreateIntelList();
    void CreateActions();
    void UpdateDisplay();
    std::string GetIntelTypeString(IntelType type) const;
    std::string GetReliabilityString(IntelReliability reliability) const;

    GUIRenderer* renderer_;
    bool visible_ = false;

    std::unique_ptr<GUIPanel> panel_;
    std::unique_ptr<GUILabel> titleLabel_;
    std::unique_ptr<GUIButton> closeBtn_;

    std::unique_ptr<GUILabel> spySectionLabel_;
    std::unique_ptr<GUIListBox> spyList_;
    std::unique_ptr<GUILabel> spyDetailLabel_;

    std::unique_ptr<GUILabel> intelSectionLabel_;
    std::unique_ptr<GUIListBox> intelList_;
    std::unique_ptr<GUILabel> intelDetailLabel_;

    std::unique_ptr<GUIButton> deploySpyBtn_;
    std::unique_ptr<GUIButton> recallSpyBtn_;
    std::unique_ptr<GUIButton> analyzeBtn_;
    std::unique_ptr<GUIButton> sweepBtn_;

    std::vector<SpyAgent> spies_;
    std::vector<IntelPiece> intelPieces_;
    SpyAgent* selectedSpy_ = nullptr;
    IntelPiece* selectedIntel_ = nullptr;

    std::function<void()> closeCallback_;
    std::function<void()> deploySpyCallback_;
    std::function<void(const std::string&)> analyzeCallback_;
};

}
