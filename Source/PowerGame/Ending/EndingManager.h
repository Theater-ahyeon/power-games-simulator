#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include "../Core/GameSystem.h"
#include "../Core/EventBus.h"

namespace PowerGame {

enum class EndingType {
    Dictatorship,
    DemocraticReform,
    PowerBalance,
    ExileFailure,
    ShadowControl,
    SystemCollapse,
    ForeignIntervention,
    PersonalAmbition
};

struct EndingCondition {
    std::string id;
    std::string description;
    EndingType type;
    std::unordered_map<std::string, float> resourceRequirements;
    std::unordered_map<std::string, float> factionRequirements;
    std::vector<std::string> requiredDecisions;
    std::vector<std::string> blockedDecisions;
    float priority;
};

struct Ending {
    EndingType type;
    std::string id;
    std::string title;
    std::string description;
    std::string epilogue;
    std::vector<std::string> triggeredBy;
    float score;
    bool unlocked;
};

class EndingManager : public IGameSystem {
public:
    EndingManager() = default;

    void Initialize() override;
    void Reset() override;
    void OnTurnUpdate(int turn) override;
    void OnFrameUpdate(float deltaTime) override {}
    std::string GetSystemName() const override { return "EndingManager"; }

    bool CheckEndings();
    const Ending* GetTriggeredEnding() const { return triggeredEnding_ ? &*triggeredEnding_ : nullptr; }
    std::vector<Ending> EvaluateAllEndings() const;

    void AddEndingCondition(const EndingCondition& condition);
    float CalculateEndingScore(EndingType type) const;
    bool IsEndingUnlocked(EndingType type) const;

    const std::unordered_map<EndingType, Ending>& GetAllEndings() const { return endings_; }
    const std::vector<EndingCondition>& GetConditions() const { return conditions_; }

    using EndingCallback = std::function<void(const Ending&)>;
    void RegisterEndingCallback(EndingCallback cb) { endingCallbacks_.push_back(cb); }

private:
    void InitializeEndings();
    void InitializeConditions();
    bool EvaluateCondition(const EndingCondition& condition) const;
    float CalculateResourceMatch(const std::unordered_map<std::string, float>& requirements) const;
    float CalculateFactionMatch(const std::unordered_map<std::string, float>& requirements) const;
    bool CheckDecisionHistory(const std::vector<std::string>& required, const std::vector<std::string>& blocked) const;

    std::unordered_map<EndingType, Ending> endings_;
    std::vector<EndingCondition> conditions_;
    std::optional<Ending> triggeredEnding_;
    std::vector<EndingCallback> endingCallbacks_;
};

}
