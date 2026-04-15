#include "GameManager.h"
#include "GameSystem.h"
#include "../Resources/ResourceManager.h"
#include "../Faction/FactionManager.h"
#include "../Intelligence/IntelligenceManager.h"
#include "../Alliance/AllianceManager.h"
#include "../Character/CharacterManager.h"
#include "../Story/StoryManager.h"
#include "../Strategy/StrategyManager.h"
#include "../Ending/EndingManager.h"

namespace PowerGame {

void GameManager::Initialize() {
    if (initialized_) return;

    auto& sysMgr = GameSystemManager::Instance();

    sysMgr.RegisterSystem<ResourceManager>();
    sysMgr.RegisterSystem<FactionManager>();
    sysMgr.RegisterSystem<IntelligenceManager>();
    sysMgr.RegisterSystem<AllianceManager>();
    sysMgr.RegisterSystem<CharacterManager>();
    sysMgr.RegisterSystem<StoryManager>();
    sysMgr.RegisterSystem<StrategyManager>();
    sysMgr.RegisterSystem<EndingManager>();

    sysMgr.InitializeAll();
    initialized_ = true;
}

void GameManager::StartNewGame() {
    currentTurn_ = 0;
    currentState_ = GameState::Playing;
    gameTime_.Reset();

    auto& sysMgr = GameSystemManager::Instance();
    sysMgr.ResetAll();

    AdvanceTurn();
}

void GameManager::AdvanceTurn() {
    currentTurn_++;
    gameTime_.Advance();

    for (auto& cb : turnCallbacks_) {
        cb(currentTurn_);
    }

    auto& sysMgr = GameSystemManager::Instance();
    sysMgr.UpdateAllTurn(currentTurn_);

    auto* endingMgr = sysMgr.GetSystem<EndingManager>();
    if (endingMgr && endingMgr->CheckEndings()) {
        SetState(GameState::Ending);
        return;
    }

    if (currentTurn_ >= maxTurns_) {
        SetState(GameState::Ending);
    }
}

void GameManager::SetState(GameState state) {
    if (currentState_ == state) return;
    currentState_ = state;
    for (auto& cb : stateCallbacks_) {
        cb(state);
    }
}

void GameManager::PauseGame() {
    SetState(GameState::Paused);
}

void GameManager::ResumeGame() {
    SetState(GameState::Playing);
}

void GameManager::EndGame() {
    currentState_ = GameState::GameOver;
    running_ = false;
    for (auto& cb : stateCallbacks_) {
        cb(GameState::GameOver);
    }
}

}
