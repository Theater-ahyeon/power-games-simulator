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

struct DecisionOption {
    std::string id;
    std::string name;
    std::string description;
    float shortTermGain;
    float longTermImpact;
    float risk;
    std::unordered_map<std::string, float> resourceEffects;
    std::unordered_map<std::string, float> factionEffects;
    std::unordered_map<std::string, float> characterEffects;
    float moralWeight;
};

struct DecisionResult {
    std::string decisionId;
    std::string optionId;
    int turnDecided;
    int turnResolved;
    bool resolved;
    float actualOutcome;
    std::string narrative;
};

struct DelayedConsequence {
    std::string id;
    std::string sourceDecisionId;
    std::string description;
    int triggerTurn;
    std::unordered_map<std::string, float> effects;
    bool triggered;
    float probability;
};

class DecisionTreeNode {
public:
    DecisionTreeNode(const std::string& id, const std::string& description)
        : id_(id), description_(description) {}

    const std::string& GetId() const { return id_; }
    const std::string& GetDescription() const { return description_; }
    const std::vector<std::shared_ptr<DecisionTreeNode>>& GetChildren() const { return children_; }
    const DecisionOption* GetOption() const { return option_ ? &*option_ : nullptr; }
    float GetProbability() const { return probability_; }
    int GetDepth() const { return depth_; }

    void AddChild(std::shared_ptr<DecisionTreeNode> child) {
        child->depth_ = depth_ + 1;
        children_.push_back(child);
    }
    void SetOption(const DecisionOption& opt) { option_ = opt; }
    void SetProbability(float prob) { probability_ = prob; }

private:
    std::string id_;
    std::string description_;
    std::vector<std::shared_ptr<DecisionTreeNode>> children_;
    std::optional<DecisionOption> option_;
    float probability_ = 1.0f;
    int depth_ = 0;
};

class DecisionTree {
public:
    DecisionTree(const std::string& id, const std::string& name)
        : id_(id), name_(name), root_(nullptr) {}

    const std::string& GetId() const { return id_; }
    const std::string& GetName() const { return name_; }
    DecisionTreeNode* GetRoot() { return root_.get(); }
    const DecisionTreeNode* GetRoot() const { return root_.get(); }

    void SetRoot(std::shared_ptr<DecisionTreeNode> root) { root_ = root; }

    DecisionTreeNode* FindNode(const std::string& nodeId);
    std::vector<DecisionTreeNode*> GetNodesAtDepth(int depth);
    int GetMaxDepth() const;
    float CalculatePathProbability(const std::vector<std::string>& path) const;

private:
    std::string id_;
    std::string name_;
    std::shared_ptr<DecisionTreeNode> root_;

    void CollectNodesAtDepth(DecisionTreeNode* node, int targetDepth, int currentDepth,
                              std::vector<DecisionTreeNode*>& result);
    int CalculateMaxDepth(const DecisionTreeNode* node) const;
};

class StrategyManager : public IGameSystem {
public:
    StrategyManager() = default;

    void Initialize() override;
    void Reset() override;
    void OnTurnUpdate(int turn) override;
    void OnFrameUpdate(float deltaTime) override {}
    std::string GetSystemName() const override { return "StrategyManager"; }

    std::string CreateDecision(const std::string& name, const std::string& description,
                                const std::vector<DecisionOption>& options);
    bool MakeDecision(const std::string& decisionId, const std::string& optionId);
    const DecisionResult* GetDecisionResult(const std::string& decisionId) const;
    const std::unordered_map<std::string, DecisionResult>& GetAllDecisions() const { return decisions_; }

    void AddDelayedConsequence(const DelayedConsequence& consequence);
    void ProcessDelayedConsequences(int currentTurn);

    DecisionTree* CreateDecisionTree(const std::string& id, const std::string& name);
    DecisionTree* GetDecisionTree(const std::string& id);
    const std::unordered_map<std::string, std::unique_ptr<DecisionTree>>& GetDecisionTrees() const { return decisionTrees_; }

    float EvaluateShortTerm(const DecisionOption& option) const;
    float EvaluateLongTerm(const DecisionOption& option) const;
    float EvaluateRisk(const DecisionOption& option) const;
    float CalculateOverallScore(const DecisionOption& option) const;

    struct DecisionAnalysis {
        float shortTermScore;
        float longTermScore;
        float riskScore;
        float overallScore;
        std::string recommendation;
    };
    DecisionAnalysis AnalyzeDecision(const DecisionOption& option) const;

    using DecisionCallback = std::function<void(const DecisionResult&)>;
    void RegisterDecisionCallback(DecisionCallback cb) { decisionCallbacks_.push_back(cb); }

private:
    void ApplyDecisionEffects(const DecisionOption& option);
    void ResolveDecision(DecisionResult& result);

    std::unordered_map<std::string, std::vector<DecisionOption>> pendingDecisions_;
    std::unordered_map<std::string, DecisionResult> decisions_;
    std::vector<DelayedConsequence> delayedConsequences_;
    std::unordered_map<std::string, std::unique_ptr<DecisionTree>> decisionTrees_;
    std::vector<DecisionCallback> decisionCallbacks_;
    int decisionIdCounter_ = 0;
};

}
