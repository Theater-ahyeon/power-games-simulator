#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include "../Core/GameSystem.h"

namespace PowerGame {

enum class UIPanelType {
    MainMenu,
    GameHUD,
    DecisionPanel,
    IntelligencePanel,
    CharacterPanel,
    ResourcePanel,
    FactionPanel,
    AlliancePanel,
    StoryPanel,
    EndingPanel,
    SettingsPanel,
    SaveLoadPanel
};

struct UIElement {
    std::string id;
    std::string text;
    float x, y, width, height;
    bool visible;
    bool interactive;
    std::string style;
};

struct UIPanel {
    UIPanelType type;
    std::string id;
    std::string title;
    float x, y, width, height;
    bool visible;
    bool modal;
    std::vector<UIElement> elements;
};

class UIManager {
public:
    static UIManager& Instance() {
        static UIManager instance;
        return instance;
    }

    void Initialize();
    void Render();
    void Update(float deltaTime);

    UIPanel* CreatePanel(UIPanelType type, const std::string& id, const std::string& title,
                          float x, float y, float w, float h);
    UIPanel* GetPanel(const std::string& id);
    void ShowPanel(const std::string& id);
    void HidePanel(const std::string& id);
    void TogglePanel(const std::string& id);

    void AddElement(const std::string& panelId, const UIElement& element);
    void UpdateElement(const std::string& panelId, const std::string& elementId, const std::string& text);

    using ClickCallback = std::function<void(const std::string& panelId, const std::string& elementId)>;
    void RegisterClickCallback(ClickCallback cb) { clickCallbacks_.push_back(cb); }

    void HandleClick(float x, float y);

private:
    UIManager() = default;
    std::unordered_map<std::string, UIPanel> panels_;
    std::vector<ClickCallback> clickCallbacks_;

    void InitializeMainMenu();
    void InitializeGameHUD();
    void InitializeDecisionPanel();
    void InitializeIntelligencePanel();
    void InitializeCharacterPanel();
    void InitializeResourcePanel();
    void InitializeFactionPanel();
    void InitializeStoryPanel();
};

class GameRenderer {
public:
    struct RenderContext {
        int screenWidth;
        int screenHeight;
        float deltaTime;
    };

    void Initialize(const RenderContext& context);
    void BeginFrame();
    void EndFrame();
    void DrawPanel(const UIPanel& panel);
    void DrawText(const std::string& text, float x, float y, const std::string& style);
    void DrawRect(float x, float y, float w, float h, const std::string& color);
    void DrawProgressBar(float x, float y, float w, float h, float progress, const std::string& color);

    const RenderContext& GetContext() const { return context_; }

private:
    RenderContext context_{1920, 1080, 0.0f};
};

}
