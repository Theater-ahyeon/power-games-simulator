#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "Core/GameManager.h"

namespace PowerGame {

enum class GamePhase {
    TITLE,
    MAIN_MENU,
    NEW_GAME,
    LOAD_GAME,
    BRIEFING,
    PLANNING,
    ACTION,
    RESOLUTION,
    EVENT,
    END_TURN,
    PAUSE,
    SETTINGS,
    SAVE,
    LOAD,
    ENDING,
    GAME_OVER
};

struct GameStatistics {
    int totalTurns = 0;
    int successfulDecisions = 0;
    int failedDecisions = 0;
    int betrayals = 0;
    int treatiesSigned = 0;
    int treatiesBroken = 0;
    int charactersRecruited = 0;
    int charactersLost = 0;
    int intelligenceGathered = 0;
    int resourcesGathered = 0;
    float highestPowerIndex = 0.0f;
    float lowestStability = 100.0f;
    int crisesSurvived = 0;
    
    std::unordered_map<std::string, int> factionRelations;
    std::unordered_map<std::string, int> eventsTriggered;
};

class GameStateManager {
public:
    static GameStateManager& Instance();
    
    void Initialize();
    void Shutdown();
    
    void StartNewGame();
    void LoadGame(const std::string& savePath);
    void SaveGame(const std::string& savePath);
    
    void NextPhase();
    void SetPhase(GamePhase phase);
    GamePhase GetCurrentPhase() const { return currentPhase_; }
    GamePhase GetPreviousPhase() const { return previousPhase_; }
    
    void ProcessTurn();
    void ProcessPhase();
    
    GameStatistics& GetStatistics() { return statistics_; }
    const GameStatistics& GetStatistics() const { return statistics_; }
    
    int GetCurrentTurn() const;
    int GetMaxTurns() const { return 30; }
    
    bool IsGameOver() const;
    std::string GetEndingId() const;
    
    void AddLog(const std::string& message);
    const std::vector<std::string>& GetLog() const { return gameLog_; }

private:
    GameStateManager() = default;
    
    GamePhase currentPhase_ = GamePhase::TITLE;
    GamePhase previousPhase_ = GamePhase::TITLE;
    int currentTurn_ = 0;
    bool gameActive_ = false;
    
    GameStatistics statistics_;
    std::vector<std::string> gameLog_;
    
    std::unordered_map<GamePhase, std::string> phaseNames_ = {
        {GamePhase::TITLE, "标题画面"},
        {GamePhase::MAIN_MENU, "主菜单"},
        {GamePhase::NEW_GAME, "新游戏"},
        {GamePhase::LOAD_GAME, "加载游戏"},
        {GamePhase::BRIEFING, "局势简报"},
        {GamePhase::PLANNING, "战略规划"},
        {GamePhase::ACTION, "行动阶段"},
        {GamePhase::RESOLUTION, "结果结算"},
        {GamePhase::EVENT, "事件处理"},
        {GamePhase::END_TURN, "回合结束"},
        {GamePhase::PAUSE, "暂停"},
        {GamePhase::SETTINGS, "设置"},
        {GamePhase::SAVE, "保存游戏"},
        {GamePhase::LOAD, "读取存档"},
        {GamePhase::ENDING, "结局"},
        {GamePhase::GAME_OVER, "游戏结束"}
    };
    
    std::unordered_map<GamePhase, std::function<void()>> phaseHandlers_;
};

}
