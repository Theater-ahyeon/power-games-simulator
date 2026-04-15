#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <random>
#include "../Core/GameSystem.h"
#include "../Core/EventBus.h"

namespace PowerGame {

enum class IntelType {
    PoliticalSecret,
    FinancialRecord,
    MilitaryIntel,
    PersonalScandal,
    AllianceDetail,
    BetrayalPlan,
    PolicyLeak,
    ForeignConnection
};

enum class IntelReliability {
    Unverified = 0,
    Low = 25,
    Medium = 50,
    High = 75,
    Confirmed = 100
};

struct IntelPiece {
    std::string id;
    IntelType type;
    IntelReliability reliability;
    std::string targetId;
    std::string sourceId;
    std::string content;
    float value;
    int discoveredTurn;
    int expiresTurn;
    bool analyzed;
    bool isDisinformation;
};

struct SpyAgent {
    std::string id;
    std::string name;
    std::string targetFactionId;
    std::string coverFactionId;
    float skill;
    float loyalty;
    float exposureRisk;
    bool active;
    int turnsEmbedded;
    std::vector<std::string> gatheredIntel;
};

struct IntelNetwork {
    std::string id;
    std::string name;
    std::string factionId;
    float coverage;
    float efficiency;
    std::vector<std::string> agentIds;
    bool active;
};

class InformationAnalyzer {
public:
    struct AnalysisResult {
        std::string intelId;
        bool isDisinformation;
        float trueProbability;
        std::vector<std::string> connections;
        std::string summary;
    };

    AnalysisResult Analyze(const IntelPiece& intel, float analyzerSkill);
    std::vector<AnalysisResult> BatchAnalyze(const std::vector<IntelPiece>& intelList, float analyzerSkill);
    float CalculateDisinformationChance(const IntelPiece& intel, float analyzerSkill) const;
};

class CounterIntelligence {
public:
    struct SweepResult {
        std::vector<std::string> discoveredSpies;
        float networkDamage;
        float securityImprovement;
    };

    SweepResult PerformSecuritySweep(const std::string& factionId, float sweepQuality);
    float CalculateDetectionChance(const SpyAgent& spy, const std::string& targetFactionId) const;
    bool AttemptCounterEspionage(const std::string& defendingFactionId, const std::string& spyId);
    void ImproveSecurity(const std::string& factionId, float improvement);
    float GetSecurityLevel(const std::string& factionId) const;

private:
    std::unordered_map<std::string, float> securityLevels_;
};

class IntelligenceManager : public IGameSystem {
public:
    IntelligenceManager() = default;

    void Initialize() override;
    void Reset() override;
    void OnTurnUpdate(int turn) override;
    void OnFrameUpdate(float deltaTime) override {}
    std::string GetSystemName() const override { return "IntelligenceManager"; }

    SpyAgent* DeploySpy(const std::string& name, const std::string& targetFactionId,
                        const std::string& coverFactionId, float skill);
    bool RecallSpy(const std::string& spyId);
    SpyAgent* GetSpy(const std::string& spyId);
    const std::unordered_map<std::string, SpyAgent>& GetAllSpies() const { return spies_; }

    IntelNetwork* CreateNetwork(const std::string& name, const std::string& factionId);
    bool AddAgentToNetwork(const std::string& networkId, const std::string& spyId);
    IntelNetwork* GetNetwork(const std::string& networkId);
    const std::unordered_map<std::string, IntelNetwork>& GetAllNetworks() const { return networks_; }

    IntelPiece GatherIntel(const std::string& spyId, IntelType type, const std::string& targetId);
    InformationAnalyzer::AnalysisResult AnalyzeIntel(const std::string& intelId, float analyzerSkill);
    const IntelPiece* GetIntel(const std::string& intelId) const;
    std::vector<IntelPiece> GetIntelByType(IntelType type) const;
    std::vector<IntelPiece> GetIntelByTarget(const std::string& targetId) const;

    CounterIntelligence::SweepResult PerformSweep(const std::string& factionId, float quality);
    float GetSecurityLevel(const std::string& factionId) const;

    InformationAnalyzer& GetAnalyzer() { return analyzer_; }
    CounterIntelligence& GetCounterIntel() { return counterIntel_; }

    using IntelCallback = std::function<void(const IntelPiece&)>;
    void RegisterIntelCallback(IntelCallback cb) { intelCallbacks_.push_back(cb); }

private:
    std::string GenerateIntelId();
    float CalculateGatherChance(const SpyAgent& spy, IntelType type) const;
    void ProcessSpyTurn(SpyAgent& spy, int turn);
    void ProcessNetworkTurn(IntelNetwork& network);
    void RemoveExpiredIntel(int currentTurn);

    std::unordered_map<std::string, SpyAgent> spies_;
    std::unordered_map<std::string, IntelNetwork> networks_;
    std::unordered_map<std::string, IntelPiece> intelDatabase_;
    InformationAnalyzer analyzer_;
    CounterIntelligence counterIntel_;
    std::vector<IntelCallback> intelCallbacks_;
    int intelIdCounter_ = 0;
};

}
