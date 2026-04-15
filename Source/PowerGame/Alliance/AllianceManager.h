#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include "../Core/GameSystem.h"
#include "../Core/EventBus.h"

namespace PowerGame {

enum class AllianceType {
    MilitaryPact,
    EconomicAlliance,
    PoliticalCoalition,
    IntelligenceSharing,
    NonAggressionPact,
    SecretTreaty,
    MarriageAlliance,
    TradeAgreement
};

enum class TreatyStatus {
    Proposed,
    Active,
    Suspended,
    Broken,
    Expired
};

struct TreatyTerm {
    std::string description;
    float commitmentLevel;
    float resourceContribution;
    int durationTurns;
};

class Treaty {
public:
    Treaty(const std::string& id, AllianceType type, const std::string& factionA, const std::string& factionB)
        : id_(id), type_(type), factionA_(factionA), factionB_(factionB) {}

    const std::string& GetId() const { return id_; }
    AllianceType GetType() const { return type_; }
    const std::string& GetFactionA() const { return factionA_; }
    const std::string& GetFactionB() const { return factionB_; }
    TreatyStatus GetStatus() const { return status_; }
    int GetRemainingTurns() const { return remainingTurns_; }
    float GetTrustLevel() const { return trustLevel_; }
    const std::vector<TreatyTerm>& GetTerms() const { return terms_; }

    void AddTerm(const TreatyTerm& term) { terms_.push_back(term); }
    void SetStatus(TreatyStatus status) { status_ = status; }
    void SetTrustLevel(float trust) { trustLevel_ = std::clamp(trust, 0.0f, 100.0f); }
    void ModifyTrust(float delta) { trustLevel_ = std::clamp(trustLevel_ + delta, 0.0f, 100.0f); }
    void SetRemainingTurns(int turns) { remainingTurns_ = turns; }
    void DecrementTurns() { if (remainingTurns_ > 0) remainingTurns_--; }

    float CalculateStability() const;
    bool IsFactionInvolved(const std::string& factionId) const;

private:
    std::string id_;
    AllianceType type_;
    std::string factionA_;
    std::string factionB_;
    TreatyStatus status_ = TreatyStatus::Proposed;
    float trustLevel_ = 50.0f;
    int remainingTurns_ = 0;
    std::vector<TreatyTerm> terms_;
};

struct BetrayalRecord {
    std::string betrayerId;
    std::string victimId;
    std::string treatyId;
    float severity;
    std::string reason;
    int turnOccurred;
    float reputationDamage;
    std::vector<std::string> affectedFactions;
};

struct TrustEvaluation {
    std::string factionA;
    std::string factionB;
    float trustScore;
    float reliabilityScore;
    float riskAssessment;
    std::vector<std::string> pastBetrayals;
};

class BetrayalSystem {
public:
    struct BetrayalConsequence {
        float reputationDamage;
        float trustDamage;
        float allianceBreakChance;
        std::vector<std::string> factionReactions;
        float resourceLoss;
    };

    BetrayalConsequence CalculateConsequences(const BetrayalRecord& betrayal) const;
    bool ExecuteBetrayal(const std::string& betrayerId, const std::string& victimId,
                         const std::string& treatyId, float severity, const std::string& reason, int turn);
    float CalculateBetrayalRisk(const std::string& factionA, const std::string& factionB) const;
    const std::vector<BetrayalRecord>& GetBetrayalHistory() const { return betrayalHistory_; }
    std::vector<BetrayalRecord> GetBetrayalsByFaction(const std::string& factionId) const;

    using BetrayalCallback = std::function<void(const BetrayalRecord&)>;
    void RegisterCallback(BetrayalCallback cb) { callbacks_.push_back(cb); }

private:
    std::vector<BetrayalRecord> betrayalHistory_;
    std::vector<BetrayalCallback> callbacks_;
};

class AllianceManager : public IGameSystem {
public:
    AllianceManager() = default;

    void Initialize() override;
    void Reset() override;
    void OnTurnUpdate(int turn) override;
    void OnFrameUpdate(float deltaTime) override {}
    std::string GetSystemName() const override { return "AllianceManager"; }

    Treaty* ProposeTreaty(AllianceType type, const std::string& factionA, const std::string& factionB,
                          const std::vector<TreatyTerm>& terms, int duration);
    bool AcceptTreaty(const std::string& treatyId);
    bool BreakTreaty(const std::string& treatyId, const std::string& breakerId);
    bool SuspendTreaty(const std::string& treatyId);
    Treaty* GetTreaty(const std::string& treatyId);
    const std::unordered_map<std::string, Treaty>& GetAllTreaties() const { return treaties_; }

    std::vector<Treaty*> GetActiveTreatiesForFaction(const std::string& factionId) const;
    std::vector<Treaty*> GetTreatiesBetweenFactions(const std::string& factionA, const std::string& factionB) const;

    TrustEvaluation EvaluateTrust(const std::string& factionA, const std::string& factionB) const;
    float GetFactionTrustLevel(const std::string& factionA, const std::string& factionB) const;
    void ModifyFactionTrust(const std::string& factionA, const std::string& factionB, float delta);

    BetrayalSystem& GetBetrayalSystem() { return betrayalSystem_; }
    bool ExecuteBetrayal(const std::string& betrayerId, const std::string& victimId,
                         const std::string& treatyId, float severity, const std::string& reason);

    using AllianceCallback = std::function<void(const Treaty&)>;
    void RegisterAllianceCallback(AllianceCallback cb) { allianceCallbacks_.push_back(cb); }

private:
    std::string GenerateTreatyId();
    void ProcessTreatyTurn(Treaty& treaty, int turn);

    std::unordered_map<std::string, Treaty> treaties_;
    std::unordered_map<std::string, float> trustMatrix_;
    BetrayalSystem betrayalSystem_;
    std::vector<AllianceCallback> allianceCallbacks_;
    int treatyIdCounter_ = 0;
};

}
