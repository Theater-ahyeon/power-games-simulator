#include "UIManager.h"
#include "../Core/GameManager.h"
#include "../Resources/ResourceManager.h"
#include "../Faction/FactionManager.h"
#include "../Character/CharacterManager.h"
#include "../Intelligence/IntelligenceManager.h"
#include "../Story/StoryManager.h"
#include <algorithm>

namespace PowerGame {

void UIManager::Initialize() {
    InitializeMainMenu();
    InitializeGameHUD();
    InitializeDecisionPanel();
    InitializeIntelligencePanel();
    InitializeCharacterPanel();
    InitializeResourcePanel();
    InitializeFactionPanel();
    InitializeStoryPanel();
}

void UIManager::InitializeMainMenu() {
    auto* panel = CreatePanel(UIPanelType::MainMenu, "main_menu", "Power Game", 0, 0, 1920, 1080);
    panel->modal = true;
    AddElement("main_menu", {"btn_new_game", "New Game", 760, 400, 400, 60, true, true, "button_primary"});
    AddElement("main_menu", {"btn_load_game", "Load Game", 760, 480, 400, 60, true, true, "button_secondary"});
    AddElement("main_menu", {"btn_settings", "Settings", 760, 560, 400, 60, true, true, "button_secondary"});
    AddElement("main_menu", {"btn_quit", "Quit", 760, 640, 400, 60, true, true, "button_danger"});
    AddElement("main_menu", {"lbl_title", "POWER GAME: The Art of Political Intrigue", 460, 200, 1000, 80, true, false, "title"});
    AddElement("main_menu", {"lbl_subtitle", "A Deep Strategy Game of Power and Betrayal", 560, 300, 800, 40, true, false, "subtitle"});
}

void UIManager::InitializeGameHUD() {
    auto* panel = CreatePanel(UIPanelType::GameHUD, "game_hud", "", 0, 0, 1920, 1080);
    AddElement("game_hud", {"lbl_turn", "Turn: 1", 10, 10, 150, 30, true, false, "hud_text"});
    AddElement("game_hud", {"lbl_season", "Season: Spring", 10, 40, 150, 30, true, false, "hud_text"});
    AddElement("game_hud", {"lbl_phase", "Phase: Morning", 10, 70, 150, 30, true, false, "hud_text"});
    AddElement("game_hud", {"btn_next_turn", "Next Turn", 1750, 10, 150, 40, true, true, "button_primary"});
    AddElement("game_hud", {"btn_resources", "Resources", 10, 1020, 120, 40, true, true, "button_tab"});
    AddElement("game_hud", {"btn_factions", "Factions", 140, 1020, 120, 40, true, true, "button_tab"});
    AddElement("game_hud", {"btn_intel", "Intelligence", 270, 1020, 120, 40, true, true, "button_tab"});
    AddElement("game_hud", {"btn_characters", "Characters", 400, 1020, 120, 40, true, true, "button_tab"});
    AddElement("game_hud", {"btn_alliances", "Alliances", 530, 1020, 120, 40, true, true, "button_tab"});
    AddElement("game_hud", {"btn_story", "Story", 660, 1020, 120, 40, true, true, "button_tab"});
    AddElement("game_hud", {"btn_menu", "Menu", 1790, 1020, 120, 40, true, true, "button_danger"});
}

void UIManager::InitializeDecisionPanel() {
    auto* panel = CreatePanel(UIPanelType::DecisionPanel, "decision_panel", "Decision", 300, 150, 1320, 780);
    panel->visible = false;
    panel->modal = true;
    AddElement("decision_panel", {"lbl_decision_title", "", 320, 170, 1280, 40, true, false, "panel_title"});
    AddElement("decision_panel", {"lbl_decision_desc", "", 320, 220, 1280, 120, true, false, "panel_text"});
    AddElement("decision_panel", {"btn_choice_1", "Option 1", 350, 400, 580, 50, true, true, "button_choice"});
    AddElement("decision_panel", {"btn_choice_2", "Option 2", 350, 470, 580, 50, true, true, "button_choice"});
    AddElement("decision_panel", {"btn_choice_3", "Option 3", 350, 540, 580, 50, true, true, "button_choice"});
    AddElement("decision_panel", {"btn_choice_4", "Option 4", 350, 610, 580, 50, true, true, "button_choice"});
    AddElement("decision_panel", {"btn_choice_5", "Option 5", 350, 680, 580, 50, true, true, "button_choice"});
    AddElement("decision_panel", {"lbl_analysis", "Decision Analysis", 960, 400, 600, 300, true, false, "analysis_text"});
}

void UIManager::InitializeIntelligencePanel() {
    auto* panel = CreatePanel(UIPanelType::IntelligencePanel, "intel_panel", "Intelligence Network", 200, 100, 1520, 880);
    panel->visible = false;
    AddElement("intel_panel", {"lbl_spies", "Active Spies", 220, 130, 700, 30, true, false, "section_title"});
    AddElement("intel_panel", {"lbl_networks", "Intel Networks", 220, 400, 700, 30, true, false, "section_title"});
    AddElement("intel_panel", {"lbl_database", "Intel Database", 960, 130, 700, 30, true, false, "section_title"});
    AddElement("intel_panel", {"btn_deploy_spy", "Deploy Spy", 220, 880, 150, 40, true, true, "button_primary"});
    AddElement("intel_panel", {"btn_create_network", "Create Network", 390, 880, 170, 40, true, true, "button_primary"});
    AddElement("intel_panel", {"btn_sweep", "Security Sweep", 580, 880, 170, 40, true, true, "button_warning"});
}

void UIManager::InitializeCharacterPanel() {
    auto* panel = CreatePanel(UIPanelType::CharacterPanel, "character_panel", "Characters", 200, 100, 1520, 880);
    panel->visible = false;
    AddElement("character_panel", {"lbl_char_list", "Character List", 220, 130, 400, 30, true, false, "section_title"});
    AddElement("character_panel", {"lbl_char_detail", "Character Details", 660, 130, 400, 30, true, false, "section_title"});
    AddElement("character_panel", {"lbl_relationships", "Relationships", 1100, 130, 400, 30, true, false, "section_title"});
}

void UIManager::InitializeResourcePanel() {
    auto* panel = CreatePanel(UIPanelType::ResourcePanel, "resource_panel", "Resources", 200, 100, 1520, 880);
    panel->visible = false;
    AddElement("resource_panel", {"lbl_political_capital", "Political Capital", 220, 150, 300, 30, true, false, "resource_label"});
    AddElement("resource_panel", {"bar_political_capital", "", 540, 150, 300, 25, true, false, "progress_bar"});
    AddElement("resource_panel", {"lbl_influence", "Influence", 220, 200, 300, 30, true, false, "resource_label"});
    AddElement("resource_panel", {"bar_influence", "", 540, 200, 300, 25, true, false, "progress_bar"});
    AddElement("resource_panel", {"lbl_information", "Information", 220, 250, 300, 30, true, false, "resource_label"});
    AddElement("resource_panel", {"bar_information", "", 540, 250, 300, 25, true, false, "progress_bar"});
    AddElement("resource_panel", {"lbl_wealth", "Wealth", 220, 300, 300, 30, true, false, "resource_label"});
    AddElement("resource_panel", {"bar_wealth", "", 540, 300, 300, 25, true, false, "progress_bar"});
    AddElement("resource_panel", {"lbl_military", "Military", 220, 350, 300, 30, true, false, "resource_label"});
    AddElement("resource_panel", {"bar_military", "", 540, 350, 300, 25, true, false, "progress_bar"});
    AddElement("resource_panel", {"lbl_reputation", "Reputation", 220, 400, 300, 30, true, false, "resource_label"});
    AddElement("resource_panel", {"bar_reputation", "", 540, 400, 300, 25, true, false, "progress_bar"});
    AddElement("resource_panel", {"lbl_legitimacy", "Legitimacy", 220, 450, 300, 30, true, false, "resource_label"});
    AddElement("resource_panel", {"bar_legitimacy", "", 540, 450, 300, 25, true, false, "progress_bar"});
    AddElement("resource_panel", {"lbl_public_support", "Public Support", 220, 500, 300, 30, true, false, "resource_label"});
    AddElement("resource_panel", {"bar_public_support", "", 540, 500, 300, 25, true, false, "progress_bar"});
    AddElement("resource_panel", {"lbl_conversions", "Resource Conversions", 220, 580, 600, 30, true, false, "section_title"});
    AddElement("resource_panel", {"btn_convert", "Convert Resources", 220, 880, 170, 40, true, true, "button_primary"});
}

void UIManager::InitializeFactionPanel() {
    auto* panel = CreatePanel(UIPanelType::FactionPanel, "faction_panel", "Factions", 200, 100, 1520, 880);
    panel->visible = false;
    AddElement("faction_panel", {"lbl_faction_list", "Faction Overview", 220, 130, 700, 30, true, false, "section_title"});
    AddElement("faction_panel", {"lbl_loyalty", "Loyalty Management", 220, 500, 700, 30, true, false, "section_title"});
    AddElement("faction_panel", {"lbl_recruitment", "Talent Recruitment", 960, 130, 700, 30, true, false, "section_title"});
}

void UIManager::InitializeStoryPanel() {
    auto* panel = CreatePanel(UIPanelType::StoryPanel, "story_panel", "Story", 300, 150, 1320, 780);
    panel->visible = false;
    panel->modal = true;
    AddElement("story_panel", {"lbl_story_title", "", 320, 170, 1280, 40, true, false, "panel_title"});
    AddElement("story_panel", {"lbl_story_desc", "", 320, 220, 1280, 200, true, false, "panel_text"});
    AddElement("story_panel", {"btn_story_choice_1", "Choice 1", 350, 500, 580, 50, true, true, "button_choice"});
    AddElement("story_panel", {"btn_story_choice_2", "Choice 2", 350, 570, 580, 50, true, true, "button_choice"});
    AddElement("story_panel", {"btn_story_choice_3", "Choice 3", 350, 640, 580, 50, true, true, "button_choice"});
    AddElement("story_panel", {"btn_story_choice_4", "Choice 4", 350, 710, 580, 50, true, true, "button_choice"});
    AddElement("story_panel", {"btn_story_choice_5", "Choice 5", 350, 780, 580, 50, true, true, "button_choice"});
}

void UIManager::Render() {
    for (auto& [id, panel] : panels_) {
        if (panel.visible) {
        }
    }
}

void UIManager::Update(float deltaTime) {
    auto& gameMgr = GameManager::Instance();
    if (auto* turnLabel = GetPanel("game_hud")) {
        UpdateElement("game_hud", "lbl_turn", "Turn: " + std::to_string(gameMgr.GetCurrentTurn()));
    }
}

UIPanel* UIManager::CreatePanel(UIPanelType type, const std::string& id, const std::string& title,
                                  float x, float y, float w, float h) {
    UIPanel panel;
    panel.type = type;
    panel.id = id;
    panel.title = title;
    panel.x = x;
    panel.y = y;
    panel.width = w;
    panel.height = h;
    panel.visible = (type == UIPanelType::MainMenu);
    panel.modal = false;
    auto& inserted = panels_[id] = panel;
    return &inserted;
}

UIPanel* UIManager::GetPanel(const std::string& id) {
    auto it = panels_.find(id);
    return it != panels_.end() ? &it->second : nullptr;
}

void UIManager::ShowPanel(const std::string& id) {
    auto it = panels_.find(id);
    if (it != panels_.end()) it->second.visible = true;
}

void UIManager::HidePanel(const std::string& id) {
    auto it = panels_.find(id);
    if (it != panels_.end()) it->second.visible = false;
}

void UIManager::TogglePanel(const std::string& id) {
    auto it = panels_.find(id);
    if (it != panels_.end()) it->second.visible = !it->second.visible;
}

void UIManager::AddElement(const std::string& panelId, const UIElement& element) {
    auto it = panels_.find(panelId);
    if (it != panels_.end()) {
        it->second.elements.push_back(element);
    }
}

void UIManager::UpdateElement(const std::string& panelId, const std::string& elementId, const std::string& text) {
    auto it = panels_.find(panelId);
    if (it == panels_.end()) return;
    for (auto& elem : it->second.elements) {
        if (elem.id == elementId) {
            elem.text = text;
            return;
        }
    }
}

void UIManager::HandleClick(float x, float y) {
    for (const auto& [panelId, panel] : panels_) {
        if (!panel.visible) continue;
        for (const auto& elem : panel.elements) {
            if (!elem.interactive) continue;
            if (x >= elem.x && x <= elem.x + elem.width && y >= elem.y && y <= elem.y + elem.height) {
                for (auto& cb : clickCallbacks_) {
                    cb(panelId, elem.id);
                }
                return;
            }
        }
    }
}

void GameRenderer::Initialize(const RenderContext& context) {
    context_ = context;
}

void GameRenderer::BeginFrame() {
}

void GameRenderer::EndFrame() {
}

void GameRenderer::DrawPanel(const UIPanel& panel) {
}

void GameRenderer::DrawText(const std::string& text, float x, float y, const std::string& style) {
}

void GameRenderer::DrawRect(float x, float y, float w, float h, const std::string& color) {
}

void GameRenderer::DrawProgressBar(float x, float y, float w, float h, float progress, const std::string& color) {
}

}
