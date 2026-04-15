#include "AllianceManager.h"
#include <algorithm>
#include <random>

namespace PowerGame {

float Treaty::CalculateStability() const {
    float baseStability = trustLevel_ * 0.4f;
    float termBonus = 0.0f;
    for (const auto& term : terms_) {
        termBonus += term.commitmentLevel * 0.1f;
    }
    float durationPenalty = remainingTurns_ > 0 ? 0.0f : -20.0f;
    return std::clamp(baseStability + termBonus + durationPenalty, 0.0f, 100.0f);
}

bool Treaty::IsFactionInvolved(const std::string& factionId) const {
    return factionA_ == factionId || factionB_ == factionId;
}

BetrayalSystem::BetrayalConsequence BetrayalSystem::CalculateConsequences(const BetrayalRecord& betrayal) const {
    BetrayalConsequence consequence;
    consequence.reputationDamage = betrayal.severity * 0.8f;
    consequence.trustDamage = betrayal.severity * 0.6f;
    consequence.allianceBreakChance = betrayal.severity * 0.01f;
    consequence.resourceLoss = betrayal.severity * 0.3f;

    if (betrayal.severity > 70.0f) {
        consequence.factionReactions = {"Diplomatic protest", "Economic sanctions", "Alliance dissolution"};
    } else if (betrayal.severity > 40.0f) {
        consequence.factionReactions = {"Formal complaint", "Trust reduction", "Treaty review"};
    } else {
        consequence.factionReactions = {"Private warning", "Increased vigilance"};
    }

    return consequence;
}

bool BetrayalSystem::ExecuteBetrayal(const std::string& betrayerId, const std::string& victimId,
                                       const std::string& treatyId, float severity, const std::string& reason, int turn) {
    BetrayalRecord record;
    record.betrayerId = betrayerId;
    record.victimId = victimId;
    record.treatyId = treatyId;
    record.severity = severity;
    record.reason = reason;
    record.turnOccurred = turn;
    record.reputationDamage = severity * 0.8f;
    record.affectedFactions = {betrayerId, victimId};

    betrayalHistory_.push_back(record);

    EventBus::Instance().Publish(Events::BetrayalOccurred, BetrayalEvent{
        betrayerId, victimId, severity, reason
    });

    for (auto& cb : callbacks_) {
        cb(record);
    }

    return true;
}

float BetrayalSystem::CalculateBetrayalRisk(const std::string& factionA, const std::string& factionB) const {
    float baseRisk = 20.0f;
    int betrayalCount = 0;
    for (const auto& record : betrayalHistory_) {
        if ((record.betrayerId == factionA && record.victimId == factionB) ||
            (record.betrayerId == factionB && record.victimId == factionA)) {
            betrayalCount++;
            baseRisk += record.severity * 0.1f;
        }
    }
    baseRisk += betrayalCount * 10.0f;
    return std::clamp(baseRisk, 0.0f, 100.0f);
}

std::vector<BetrayalRecord> BetrayalSystem::GetBetrayalsByFaction(const std::string& factionId) const {
    std::vector<BetrayalRecord> result;
    for (const auto& record : betrayalHistory_) {
        if (record.betrayerId == factionId || record.victimId == factionId) {
            result.push_back(record);
        }
    }
    return result;
}

void AllianceManager::Initialize() {
}

void AllianceManager::Reset() {
    treaties_.clear();
    trustMatrix_.clear();
    betrayalSystem_ = BetrayalSystem();
    treatyIdCounter_ = 0;
}

void AllianceManager::OnTurnUpdate(int turn) {
    for (auto& [id, treaty] : treaties_) {
        if (treaty.GetStatus() == TreatyStatus::Active) {
            ProcessTreatyTurn(treaty, turn);
        }
    }
}

std::string AllianceManager::GenerateTreatyId() {
    return "treaty_" + std::to_string(++treatyIdCounter_);
}

void AllianceManager::ProcessTreatyTurn(Treaty& treaty, int turn) {
    treaty.DecrementTurns();

    float trustDrift = -0.5f;
    for (const auto& term : treaty.GetTerms()) {
        if (term.commitmentLevel > 0.5f) {
            trustDrift += 0.3f;
        }
    }
    treaty.ModifyTrust(trustDrift);

    if (treaty.GetTrustLevel() < 10.0f) {
        treaty.SetStatus(TreatyStatus::Broken);
        EventBus::Instance().Publish(Events::AllianceBroken, AllianceEvent{
            treaty.GetId(), treaty.GetFactionA(), treaty.GetFactionB()
        });
    }

    if (treaty.GetRemainingTurns() == 0 && treaty.GetStatus() == TreatyStatus::Active) {
        treaty.SetStatus(TreatyStatus::Expired);
    }
}

Treaty* AllianceManager::ProposeTreaty(AllianceType type, const std::string& factionA,
                                        const std::string& factionB,
                                        const std::vector<TreatyTerm>& terms, int duration) {
    std::string id = GenerateTreatyId();
    Treaty treaty(id, type, factionA, factionB);
    for (const auto& term : terms) {
        treaty.AddTerm(term);
    }
    treaty.SetRemainingTurns(duration);
    treaty.SetTrustLevel(GetFactionTrustLevel(factionA, factionB));

    auto& inserted = treaties_[id] = treaty;
    return &inserted;
}

bool AllianceManager::AcceptTreaty(const std::string& treatyId) {
    auto it = treaties_.find(treatyId);
    if (it == treaties_.end()) return false;
    if (it->second.GetStatus() != TreatyStatus::Proposed) return false;

    it->second.SetStatus(TreatyStatus::Active);
    ModifyFactionTrust(it->second.GetFactionA(), it->second.GetFactionB(), 5.0f);

    EventBus::Instance().Publish(Events::AllianceFormed, AllianceEvent{
        treatyId, it->second.GetFactionA(), it->second.GetFactionB()
    });

    for (auto& cb : allianceCallbacks_) {
        cb(it->second);
    }

    return true;
}

bool AllianceManager::BreakTreaty(const std::string& treatyId, const std::string& breakerId) {
    auto it = treaties_.find(treatyId);
    if (it == treaties_.end()) return false;
    if (it->second.GetStatus() != TreatyStatus::Active) return false;

    it->second.SetStatus(TreatyStatus::Broken);

    std::string victimId = it->second.GetFactionA() == breakerId ?
        it->second.GetFactionB() : it->second.GetFactionA();

    ModifyFactionTrust(breakerId, victimId, -30.0f);

    EventBus::Instance().Publish(Events::AllianceBroken, AllianceEvent{
        treatyId, breakerId, victimId
    });

    return true;
}

bool AllianceManager::SuspendTreaty(const std::string& treatyId) {
    auto it = treaties_.find(treatyId);
    if (it == treaties_.end()) return false;
    if (it->second.GetStatus() != TreatyStatus::Active) return false;

    it->second.SetStatus(TreatyStatus::Suspended);
    return true;
}

Treaty* AllianceManager::GetTreaty(const std::string& treatyId) {
    auto it = treaties_.find(treatyId);
    return it != treaties_.end() ? &it->second : nullptr;
}

std::vector<Treaty*> AllianceManager::GetActiveTreatiesForFaction(const std::string& factionId) const {
    std::vector<Treaty*> result;
    for (auto& [id, treaty] : treaties_) {
        if (treaty.GetStatus() == TreatyStatus::Active && treaty.IsFactionInvolved(factionId)) {
            result.push_back(const_cast<Treaty*>(&treaty));
        }
    }
    return result;
}

std::vector<Treaty*> AllianceManager::GetTreatiesBetweenFactions(const std::string& factionA,
                                                                   const std::string& factionB) const {
    std::vector<Treaty*> result;
    for (auto& [id, treaty] : treaties_) {
        if (treaty.IsFactionInvolved(factionA) && treaty.IsFactionInvolved(factionB)) {
            result.push_back(const_cast<Treaty*>(&treaty));
        }
    }
    return result;
}

TrustEvaluation AllianceManager::EvaluateTrust(const std::string& factionA, const std::string& factionB) const {
    TrustEvaluation eval;
    eval.factionA = factionA;
    eval.factionB = factionB;
    eval.trustScore = GetFactionTrustLevel(factionA, factionB);
    eval.riskAssessment = betrayalSystem_.CalculateBetrayalRisk(factionA, factionB);

    float reliability = eval.trustScore * 0.6f + (100.0f - eval.riskAssessment) * 0.4f;
    eval.reliabilityScore = std::clamp(reliability, 0.0f, 100.0f);

    auto betrayals = betrayalSystem_.GetBetrayalsByFaction(factionA);
    for (const auto& b : betrayals) {
        if (b.victimId == factionB) {
            eval.pastBetrayals.push_back(b.treatyId);
        }
    }

    return eval;
}

float AllianceManager::GetFactionTrustLevel(const std::string& factionA, const std::string& factionB) const {
    std::string key = factionA < factionB ? factionA + ":" + factionB : factionB + ":" + factionA;
    auto it = trustMatrix_.find(key);
    return it != trustMatrix_.end() ? it->second : 50.0f;
}

void AllianceManager::ModifyFactionTrust(const std::string& factionA, const std::string& factionB, float delta) {
    std::string key = factionA < factionB ? factionA + ":" + factionB : factionB + ":" + factionA;
    trustMatrix_[key] = std::clamp(GetFactionTrustLevel(factionA, factionB) + delta, 0.0f, 100.0f);
}

bool AllianceManager::ExecuteBetrayal(const std::string& betrayerId, const std::string& victimId,
                                       const std::string& treatyId, float severity, const std::string& reason) {
    auto& gameMgr = GameManager::Instance();
    betrayalSystem_.ExecuteBetrayal(betrayerId, victimId, treatyId, severity, reason, gameMgr.GetCurrentTurn());

    BreakTreaty(treatyId, betrayerId);
    ModifyFactionTrust(betrayerId, victimId, -severity * 0.5f);

    return true;
}

}
