#include "GameStateManager.h"
#include "Core/GameManager.h"
#include "Resources/ResourceManager.h"
#include "Faction/FactionManager.h"
#include "Intelligence/IntelligenceManager.h"
#include "Character/CharacterManager.h"
#include "Story/StoryManager.h"
#include "Strategy/StrategyManager.h"
#include "Ending/EndingManager.h"
#include "AchievementSystem.h"

namespace PowerGame {

GameStateManager& GameStateManager::Instance() {
    static GameStateManager instance;
    return instance;
}

void GameStateManager::Initialize() {
    currentPhase_ = GamePhase::TITLE;
    previousPhase_ = GamePhase::TITLE;
    currentTurn_ = 0;
    gameActive_ = false;
    
    phaseHandlers_[GamePhase::BRIEFING] = [this]() {
        AddLog("=== 回合 " + std::to_string(currentTurn_) + " 局势简报 ===");
    };
    
    phaseHandlers_[GamePhase::PLANNING] = [this]() {
        AddLog("进入战略规划阶段");
    };
    
    phaseHandlers_[GamePhase::ACTION] = [this]() {
        AddLog("行动阶段：执行玩家指令");
    };
    
    phaseHandlers_[GamePhase::RESOLUTION] = [this]() {
        AddLog("结算行动结果");
        AchievementSystem::Instance().NotifyEvent("turn_complete", "");
    };
    
    phaseHandlers_[GamePhase::EVENT] = [this]() {
        AddLog("处理随机事件");
        auto& storyMgr = StoryManager::Instance();
        storyMgr.ProcessRandomEvent();
    };
    
    phaseHandlers_[GamePhase::END_TURN] = [this]() {
        AddLog("回合结束处理");
        
        auto& gameMgr = GameManager::Instance();
        gameMgr.ProcessTurnEnd();
        
        statistics_.totalTurns = currentTurn_;
        
        AchievementSystem::Instance().CheckAllAchievements();
    };
    
    phaseHandlers_[GamePhase::ENDING] = [this]() {
        auto& endingMgr = EndingManager::Instance();
        std::string endingId = endingMgr.DetermineEnding();
        AddLog("游戏结束 - " + endingMgr.GetEndingName(endingId));
    };
}

void GameStateManager::Shutdown() {
    phaseHandlers_.clear();
    gameLog_.clear();
    statistics_ = GameStatistics();
}

void GameStateManager::StartNewGame() {
    auto& gameMgr = GameManager::Instance();
    gameMgr.Initialize();
    
    currentTurn_ = 1;
    gameActive_ = true;
    statistics_ = GameStatistics();
    gameLog_.clear();
    
    AddLog("=== 新游戏开始 ===");
    AddLog("回合 1：权力真空");
    
    SetPhase(GamePhase::BRIEFING);
}

void GameStateManager::LoadGame(const std::string& savePath) {
    AddLog("加载存档：" + savePath);
}

void GameStateManager::SaveGame(const std::string& savePath) {
    AddLog("保存存档：" + savePath);
}

void GameStateManager::NextPhase() {
    previousPhase_ = currentPhase_;
    
    switch (currentPhase_) {
        case GamePhase::TITLE:
            SetPhase(GamePhase::MAIN_MENU);
            break;
        case GamePhase::MAIN_MENU:
            break;
        case GamePhase::BRIEFING:
            SetPhase(GamePhase::PLANNING);
            break;
        case GamePhase::PLANNING:
            SetPhase(GamePhase::ACTION);
            break;
        case GamePhase::ACTION:
            SetPhase(GamePhase::RESOLUTION);
            break;
        case GamePhase::RESOLUTION:
            SetPhase(GamePhase::EVENT);
            break;
        case GamePhase::EVENT:
            SetPhase(GamePhase::END_TURN);
            break;
        case GamePhase::END_TURN:
            currentTurn_++;
            if (currentTurn_ > GetMaxTurns() || IsGameOver()) {
                SetPhase(GamePhase::ENDING);
            } else {
                SetPhase(GamePhase::BRIEFING);
            }
            break;
        default:
            break;
    }
    
    if (phaseHandlers_.find(currentPhase_) != phaseHandlers_.end()) {
        phaseHandlers_[currentPhase_]();
    }
}

void GameStateManager::SetPhase(GamePhase phase) {
    previousPhase_ = currentPhase_;
    currentPhase_ = phase;
}

void GameStateManager::ProcessTurn() {
    if (!gameActive_) return;
    
    while (currentPhase_ != GamePhase::ENDING && currentPhase_ != GamePhase::GAME_OVER) {
        ProcessPhase();
    }
}

void GameStateManager::ProcessPhase() {
    NextPhase();
}

int GameStateManager::GetCurrentTurn() const {
    return currentTurn_;
}

bool GameStateManager::IsGameOver() const {
    auto& resourceMgr = ResourceManager::Instance();
    auto& factionMgr = FactionManager::Instance();
    
    if (resourceMgr.GetResource("PoliticalCapital") <= 0) {
        return true;
    }
    
    if (resourceMgr.GetResource("Legitimacy") <= 10) {
        return true;
    }
    
    int hostileCount = 0;
    for (int i = 0; i < 5; ++i) {
        if (factionMgr.GetFactionTrust(i) < 20) {
            hostileCount++;
        }
    }
    if (hostileCount >= 4) {
        return true;
    }
    
    return false;
}

std::string GameStateManager::GetEndingId() const {
    auto& endingMgr = EndingManager::Instance();
    return endingMgr.DetermineEnding();
}

void GameStateManager::AddLog(const std::string& message) {
    gameLog_.push_back(message);
    if (gameLog_.size() > 100) {
        gameLog_.erase(gameLog_.begin());
    }
}

}
