#include "IntelligenceManager.h"
#include <algorithm>
#include <random>

namespace PowerGame {

InformationAnalyzer::AnalysisResult InformationAnalyzer::Analyze(const IntelPiece& intel, float analyzerSkill) {
    AnalysisResult result;
    result.intelId = intel.id;
    result.isDisinformation = intel.isDisinformation;
    result.trueProbability = 1.0f - CalculateDisinformationChance(intel, analyzerSkill);
    result.summary = intel.content;
    return result;
}

std::vector<InformationAnalyzer::AnalysisResult> InformationAnalyzer::BatchAnalyze(
    const std::vector<IntelPiece>& intelList, float analyzerSkill) {
    std::vector<AnalysisResult> results;
    results.reserve(intelList.size());
    for (const auto& intel : intelList) {
        results.push_back(Analyze(intel, analyzerSkill));
    }
    return results;
}

float InformationAnalyzer::CalculateDisinformationChance(const IntelPiece& intel, float analyzerSkill) const {
    float baseChance = intel.isDisinformation ? 0.8f : 0.1f;
    float skillModifier = analyzerSkill / 100.0f;
    float reliabilityModifier = static_cast<float>(intel.reliability) / 100.0f;
    float detectionChance = baseChance * (1.0f - skillModifier * 0.5f) * (1.0f - reliabilityModifier * 0.3f);
    return std::clamp(detectionChance, 0.0f, 1.0f);
}

CounterIntelligence::SweepResult CounterIntelligence::PerformSecuritySweep(
    const std::string& factionId, float sweepQuality) {
    SweepResult result;
    result.networkDamage = sweepQuality * 0.3f;
    result.securityImprovement = sweepQuality * 0.2f;
    ImproveSecurity(factionId, result.securityImprovement);
    return result;
}

float CounterIntelligence::CalculateDetectionChance(const SpyAgent& spy, const std::string& targetFactionId) const {
    float security = GetSecurityLevel(targetFactionId);
    float exposure = spy.exposureRisk;
    float embeddedBonus = std::min(spy.turnsEmbedded * 0.02f, 0.2f);
    float detectionChance = (security * 0.4f + exposure * 0.4f) * (1.0f - embeddedBonus);
    return std::clamp(detectionChance, 0.05f, 0.95f);
}

bool CounterIntelligence::AttemptCounterEspionage(const std::string& defendingFactionId, const std::string& spyId) {
    return false;
}

void CounterIntelligence::ImproveSecurity(const std::string& factionId, float improvement) {
    securityLevels_[factionId] = std::clamp(GetSecurityLevel(factionId) + improvement, 0.0f, 100.0f);
}

float CounterIntelligence::GetSecurityLevel(const std::string& factionId) const {
    auto it = securityLevels_.find(factionId);
    return it != securityLevels_.end() ? it->second : 30.0f;
}

void IntelligenceManager::Initialize() {
    counterIntel_.ImproveSecurity("conservatives", 40.0f);
    counterIntel_.ImproveSecurity("liberals", 35.0f);
    counterIntel_.ImproveSecurity("neutrals", 30.0f);
    counterIntel_.ImproveSecurity("radicals", 45.0f);
    counterIntel_.ImproveSecurity("opportunists", 25.0f);
}

void IntelligenceManager::Reset() {
    spies_.clear();
    networks_.clear();
    intelDatabase_.clear();
    intelIdCounter_ = 0;
}

void IntelligenceManager::OnTurnUpdate(int turn) {
    for (auto& [id, spy] : spies_) {
        if (spy.active) {
            ProcessSpyTurn(spy, turn);
        }
    }
    for (auto& [id, network] : networks_) {
        if (network.active) {
            ProcessNetworkTurn(network);
        }
    }
    RemoveExpiredIntel(turn);
}

std::string IntelligenceManager::GenerateIntelId() {
    return "intel_" + std::to_string(++intelIdCounter_);
}

float IntelligenceManager::CalculateGatherChance(const SpyAgent& spy, IntelType type) const {
    float base = spy.skill * 0.5f;
    float embeddedBonus = std::min(spy.turnsEmbedded * 0.03f, 0.3f);
    float riskPenalty = spy.exposureRisk * 0.2f;
    return std::clamp(base + embeddedBonus - riskPenalty, 0.05f, 0.95f);
}

void IntelligenceManager::ProcessSpyTurn(SpyAgent& spy, int turn) {
    spy.turnsEmbedded++;
    spy.exposureRisk += 0.5f + static_cast<float>(turn) * 0.01f;
    spy.exposureRisk = std::min(spy.exposureRisk, 100.0f);

    float detectionChance = counterIntel_.CalculateDetectionChance(spy, spy.targetFactionId);
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    if (dist(rng) < detectionChance) {
        spy.active = false;
        EventBus::Instance().Publish(Events::SpyDiscovered, IntelligenceEvent{
            spy.id, spy.targetFactionId, "SpyDiscovered", spy.skill
        });
    }
}

void IntelligenceManager::ProcessNetworkTurn(IntelNetwork& network) {
    float agentContribution = 0.0f;
    for (const auto& agentId : network.agentIds) {
        auto it = spies_.find(agentId);
        if (it != spies_.end() && it->second.active) {
            agentContribution += it->second.skill * 0.1f;
        }
    }
    network.efficiency = std::clamp(30.0f + agentContribution, 0.0f, 100.0f);
    network.coverage = std::min(network.coverage + network.efficiency * 0.01f, 100.0f);
}

void IntelligenceManager::RemoveExpiredIntel(int currentTurn) {
    std::vector<std::string> expired;
    for (const auto& [id, intel] : intelDatabase_) {
        if (intel.expiresTurn > 0 && intel.expiresTurn <= currentTurn) {
            expired.push_back(id);
        }
    }
    for (const auto& id : expired) {
        intelDatabase_.erase(id);
    }
}

SpyAgent* IntelligenceManager::DeploySpy(const std::string& name, const std::string& targetFactionId,
                                          const std::string& coverFactionId, float skill) {
    SpyAgent spy;
    spy.id = "spy_" + std::to_string(spies_.size() + 1);
    spy.name = name;
    spy.targetFactionId = targetFactionId;
    spy.coverFactionId = coverFactionId;
    spy.skill = skill;
    spy.loyalty = 70.0f;
    spy.exposureRisk = 10.0f;
    spy.active = true;
    spy.turnsEmbedded = 0;

    auto& inserted = spies_[spy.id] = spy;
    return &inserted;
}

bool IntelligenceManager::RecallSpy(const std::string& spyId) {
    auto it = spies_.find(spyId);
    if (it == spies_.end()) return false;
    it->second.active = false;
    return true;
}

SpyAgent* IntelligenceManager::GetSpy(const std::string& spyId) {
    auto it = spies_.find(spyId);
    return it != spies_.end() ? &it->second : nullptr;
}

IntelNetwork* IntelligenceManager::CreateNetwork(const std::string& name, const std::string& factionId) {
    IntelNetwork network;
    network.id = "net_" + std::to_string(networks_.size() + 1);
    network.name = name;
    network.factionId = factionId;
    network.coverage = 10.0f;
    network.efficiency = 30.0f;
    network.active = true;

    auto& inserted = networks_[network.id] = network;
    return &inserted;
}

bool IntelligenceManager::AddAgentToNetwork(const std::string& networkId, const std::string& spyId) {
    auto netIt = networks_.find(networkId);
    if (netIt == networks_.end()) return false;
    auto spyIt = spies_.find(spyId);
    if (spyIt == spies_.end()) return false;

    netIt->second.agentIds.push_back(spyId);
    return true;
}

IntelNetwork* IntelligenceManager::GetNetwork(const std::string& networkId) {
    auto it = networks_.find(networkId);
    return it != networks_.end() ? &it->second : nullptr;
}

IntelPiece IntelligenceManager::GatherIntel(const std::string& spyId, IntelType type, const std::string& targetId) {
    IntelPiece intel;
    intel.id = GenerateIntelId();
    intel.type = type;
    intel.targetId = targetId;
    intel.sourceId = spyId;
    intel.reliability = IntelReliability::Unverified;
    intel.value = 10.0f;
    intel.discoveredTurn = 0;
    intel.expiresTurn = 0;
    intel.analyzed = false;
    intel.isDisinformation = false;

    auto it = spies_.find(spyId);
    if (it != spies_.end() && it->second.active) {
        float chance = CalculateGatherChance(it->second, type);
        static std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);

        if (dist(rng) < chance) {
            intel.reliability = static_cast<IntelReliability>(
                static_cast<int>(IntelReliability::Low) +
                static_cast<int>(it->second.skill * 0.5f));
            intel.value = it->second.skill * 0.5f + 10.0f;
            intel.content = "Intel gathered by " + it->second.name + " about " + targetId;

            std::uniform_real_distribution<float> disinfoDist(0.0f, 1.0f);
            intel.isDisinformation = disinfoDist(rng) < 0.15f;

            it->second.gatheredIntel.push_back(intel.id);
            it->second.exposureRisk += 5.0f;

            EventBus::Instance().Publish(Events::IntelligenceGathered, IntelligenceEvent{
                spyId, targetId, "Gathered", intel.value
            });
        }
    }

    intelDatabase_[intel.id] = intel;
    for (auto& cb : intelCallbacks_) {
        cb(intel);
    }
    return intel;
}

InformationAnalyzer::AnalysisResult IntelligenceManager::AnalyzeIntel(const std::string& intelId, float analyzerSkill) {
    auto it = intelDatabase_.find(intelId);
    if (it == intelDatabase_.end()) {
        return InformationAnalyzer::AnalysisResult{};
    }
    it->second.analyzed = true;
    return analyzer_.Analyze(it->second, analyzerSkill);
}

const IntelPiece* IntelligenceManager::GetIntel(const std::string& intelId) const {
    auto it = intelDatabase_.find(intelId);
    return it != intelDatabase_.end() ? &it->second : nullptr;
}

std::vector<IntelPiece> IntelligenceManager::GetIntelByType(IntelType type) const {
    std::vector<IntelPiece> result;
    for (const auto& [id, intel] : intelDatabase_) {
        if (intel.type == type) result.push_back(intel);
    }
    return result;
}

std::vector<IntelPiece> IntelligenceManager::GetIntelByTarget(const std::string& targetId) const {
    std::vector<IntelPiece> result;
    for (const auto& [id, intel] : intelDatabase_) {
        if (intel.targetId == targetId) result.push_back(intel);
    }
    return result;
}

CounterIntelligence::SweepResult IntelligenceManager::PerformSweep(const std::string& factionId, float quality) {
    return counterIntel_.PerformSecuritySweep(factionId, quality);
}

float IntelligenceManager::GetSecurityLevel(const std::string& factionId) const {
    return counterIntel_.GetSecurityLevel(factionId);
}

}
