#pragma once
#include <string>
#include <functional>
#include <memory>

namespace PowerGame {

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    StoryEvent,
    DecisionMaking,
    Ending,
    GameOver
};

enum class Season { Spring, Summer, Autumn, Winter };
enum class Phase { Morning, Afternoon, Evening, Night };

class GameTime {
public:
    GameTime() = default;

    int GetYear() const { return year_; }
    Season GetSeason() const { return season_; }
    Phase GetPhase() const { return phase_; }
    int GetTotalTurns() const { return totalTurns_; }

    void Advance() {
        totalTurns_++;
        int nextPhase = static_cast<int>(phase_) + 1;
        if (nextPhase > static_cast<int>(Phase::Night)) {
            phase_ = Phase::Morning;
            int nextSeason = static_cast<int>(season_) + 1;
            if (nextSeason > static_cast<int>(Season::Winter)) {
                season_ = Season::Spring;
                year_++;
            } else {
                season_ = static_cast<Season>(nextSeason);
            }
        } else {
            phase_ = static_cast<Phase>(nextPhase);
        }
    }

    float GetSeasonModifier(const std::string& resourceType) const {
        switch (season_) {
            case Season::Spring:
                return resourceType == "Influence" ? 1.2f : 1.0f;
            case Season::Summer:
                return resourceType == "PoliticalCapital" ? 1.2f : 1.0f;
            case Season::Autumn:
                return resourceType == "Information" ? 1.3f : 1.0f;
            case Season::Winter:
                return resourceType == "Military" ? 1.2f : 0.9f;
            default:
                return 1.0f;
        }
    }

    void Reset() {
        year_ = 1;
        season_ = Season::Spring;
        phase_ = Phase::Morning;
        totalTurns_ = 0;
    }

private:
    int year_ = 1;
    Season season_ = Season::Spring;
    Phase phase_ = Phase::Morning;
    int totalTurns_ = 0;
};

class GameManager {
public:
    static GameManager& Instance() {
        static GameManager instance;
        return instance;
    }

    void Initialize();
    void StartNewGame();
    void AdvanceTurn();
    void SetState(GameState state);
    void PauseGame();
    void ResumeGame();
    void EndGame();

    GameState GetCurrentState() const { return currentState_; }
    int GetCurrentTurn() const { return currentTurn_; }
    int GetMaxTurns() const { return maxTurns_; }
    float GetGameSpeed() const { return gameSpeed_; }
    const GameTime& GetGameTime() const { return gameTime_; }
    bool IsInitialized() const { return initialized_; }

    void SetGameSpeed(float speed) { gameSpeed_ = speed; }

    using StateCallback = std::function<void(GameState)>;
    using TurnCallback = std::function<void(int)>;

    void RegisterStateCallback(StateCallback cb) { stateCallbacks_.push_back(cb); }
    void RegisterTurnCallback(TurnCallback cb) { turnCallbacks_.push_back(cb); }

private:
    GameManager() = default;

    GameState currentState_ = GameState::MainMenu;
    int currentTurn_ = 0;
    int maxTurns_ = 100;
    float gameSpeed_ = 1.0f;
    GameTime gameTime_;
    bool initialized_ = false;
    bool running_ = false;

    std::vector<StateCallback> stateCallbacks_;
    std::vector<TurnCallback> turnCallbacks_;
};

}
