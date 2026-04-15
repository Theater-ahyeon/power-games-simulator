#include "StrategyManager.h"
#include "../Core/GameManager.h"
#include "../Resources/ResourceManager.h"
#include <algorithm>
#include <random>
#include <cmath>

namespace PowerGame {

DecisionTreeNode* DecisionTree::FindNode(const std::string& nodeId) {
    if (!root_) return nullptr;
    std::vector<DecisionTreeNode*> stack = {root_.get()};
    while (!stack.empty()) {
        auto* current = stack.back();
        stack.pop_back();
        if (current->GetId() == nodeId) return current;
        for (auto& child : current->GetChildren()) {
            stack.push_back(child.get());
        }
    }
    return nullptr;
}

std::vector<DecisionTreeNode*> DecisionTree::GetNodesAtDepth(int depth) {
    std::vector<DecisionTreeNode*> result;
    if (root_) CollectNodesAtDepth(root_.get(), depth, 0, result);
    return result;
}

void DecisionTree::CollectNodesAtDepth(DecisionTreeNode* node, int targetDepth, int currentDepth,
                                         std::vector<DecisionTreeNode*>& result) {
    if (currentDepth == targetDepth) {
        result.push_back(node);
        return;
    }
    for (auto& child : node->GetChildren()) {
        CollectNodesAtDepth(child.get(), targetDepth, currentDepth + 1, result);
    }
}

int DecisionTree::GetMaxDepth() const {
    return root_ ? CalculateMaxDepth(root_.get()) : 0;
}

int DecisionTree::CalculateMaxDepth(const DecisionTreeNode* node) const {
    if (!node || node->GetChildren().empty()) return node ? node->GetDepth() : 0;
    int maxDepth = node->GetDepth();
    for (const auto& child : node->GetChildren()) {
        maxDepth = std::max(maxDepth, CalculateMaxDepth(child.get()));
    }
    return maxDepth;
}

float DecisionTree::CalculatePathProbability(const std::vector<std::string>& path) const {
    if (!root_ || path.empty()) return 0.0f;
    float probability = 1.0f;
    const DecisionTreeNode* current = root_.get();
    for (const auto& nodeId : path) {
        bool found = false;
        for (const auto& child : current->GetChildren()) {
            if (child->GetId() == nodeId) {
                probability *= child->GetProbability();
                current = child.get();
                found = true;
                break;
            }
        }
        if (!found) return 0.0f;
    }
    return probability;
}

void StrategyManager::Initialize() {
    auto tree = CreateDecisionTree("main_strategy", "Main Strategy Tree");

    auto root = std::make_shared<DecisionTreeNode>("root", "Strategic Decision Point");
    root->SetOption({"root", "Initial Assessment", "Evaluate the political landscape", 0, 0, 0, {}, {}, {}, 0.5f});

    auto consolidate = std::make_shared<DecisionTreeNode>("consolidate", "Consolidate Power Path");
    consolidate->SetProbability(0.4f);
    consolidate->SetOption({"consolidate", "Consolidate Power", "Focus on centralizing authority",
        20, -10, 40, {{"PoliticalCapital", 15}, {"Reputation", -10}}, {{"conservatives", 10}}, {}, 0.3f});

    auto reform = std::make_shared<DecisionTreeNode>("reform", "Reform Path");
    reform->SetProbability(0.35f);
    reform->SetOption({"reform", "Push Reform", "Advocate for systemic change",
        -5, 25, 50, {{"PoliticalCapital", -10}, {"PublicSupport", 15}}, {{"liberals", 15}}, {}, 0.7f});

    auto balance = std::make_shared<DecisionTreeNode>("balance", "Balance Path");
    balance->SetProbability(0.25f);
    balance->SetOption({"balance", "Maintain Balance", "Keep factions in equilibrium",
        5, 15, 20, {{"Influence", 10}}, {{"neutrals", 10}}, {}, 0.6f});

    auto consolidateDeep1 = std::make_shared<DecisionTreeNode>("consolidate_deep1", "Deep Consolidation");
    consolidateDeep1->SetProbability(0.6f);
    consolidateDeep1->SetOption({"consolidate_deep1", "Full Authoritarian Control", "Eliminate all opposition",
        30, -30, 60, {{"PoliticalCapital", 25}, {"Legitimacy", -20}}, {{"conservatives", 15}, {"radicals", -20}}, {}, 0.2f});

    auto consolidateDeep2 = std::make_shared<DecisionTreeNode>("consolidate_deep2", "Moderate Consolidation");
    consolidateDeep2->SetProbability(0.4f);
    consolidateDeep2->SetOption({"consolidate_deep2", "Soft Power Consolidation", "Gradually increase control",
        15, 5, 25, {{"PoliticalCapital", 10}, {"Influence", 10}}, {}, {}, 0.5f});

    consolidate->AddChild(consolidateDeep1);
    consolidate->AddChild(consolidateDeep2);

    auto reformDeep1 = std::make_shared<DecisionTreeNode>("reform_deep1", "Radical Reform");
    reformDeep1->SetProbability(0.5f);
    reformDeep1->SetOption({"reform_deep1", "Constitutional Overhaul", "Rewrite the fundamental law",
        -15, 35, 55, {{"PoliticalCapital", -20}, {"Legitimacy", 20}}, {{"liberals", 20}, {"conservatives", -15}}, {}, 0.7f});

    auto reformDeep2 = std::make_shared<DecisionTreeNode>("reform_deep2", "Gradual Reform");
    reformDeep2->SetProbability(0.5f);
    reformDeep2->SetOption({"reform_deep2", "Incremental Changes", "Step-by-step reform process",
        5, 20, 30, {{"PoliticalCapital", -5}, {"PublicSupport", 10}}, {{"neutrals", 10}}, {}, 0.6f});

    reform->AddChild(reformDeep1);
    reform->AddChild(reformDeep2);

    root->AddChild(consolidate);
    root->AddChild(reform);
    root->AddChild(balance);

    tree->SetRoot(root);
}

void StrategyManager::Reset() {
    pendingDecisions_.clear();
    decisions_.clear();
    delayedConsequences_.clear();
    decisionIdCounter_ = 0;
}

void StrategyManager::OnTurnUpdate(int turn) {
    ProcessDelayedConsequences(turn);

    for (auto& [id, result] : decisions_) {
        if (!result.resolved && turn >= result.turnResolved) {
            ResolveDecision(result);
        }
    }
}

std::string StrategyManager::CreateDecision(const std::string& name, const std::string& description,
                                              const std::vector<DecisionOption>& options) {
    std::string id = "decision_" + std::to_string(++decisionIdCounter_);
    pendingDecisions_[id] = options;
    return id;
}

bool StrategyManager::MakeDecision(const std::string& decisionId, const std::string& optionId) {
    auto it = pendingDecisions_.find(decisionId);
    if (it == pendingDecisions_.end()) return false;

    auto& options = it->second;
    auto optIt = std::find_if(options.begin(), options.end(),
        [&optionId](const DecisionOption& o) { return o.id == optionId; });
    if (optIt == options.end()) return false;

    DecisionResult result;
    result.decisionId = decisionId;
    result.optionId = optionId;
    result.turnDecided = GameManager::Instance().GetCurrentTurn();
    result.turnResolved = result.turnDecided + 3;
    result.resolved = false;
    result.actualOutcome = 0.0f;

    decisions_[decisionId] = result;

    ApplyDecisionEffects(*optIt);

    if (optIt->longTermImpact != 0) {
        DelayedConsequence consequence;
        consequence.id = "delayed_" + decisionId;
        consequence.sourceDecisionId = decisionId;
        consequence.description = "Long-term effects of " + optIt->name;
        consequence.triggerTurn = result.turnDecided + 5;
        consequence.effects = optIt->resourceEffects;
        consequence.triggered = false;
        consequence.probability = 0.8f;
        AddDelayedConsequence(consequence);
    }

    EventBus::Instance().Publish(Events::DecisionMade, StoryEvent{decisionId, optionId, result.turnDecided});
    for (auto& cb : decisionCallbacks_) {
        cb(result);
    }

    pendingDecisions_.erase(it);
    return true;
}

const DecisionResult* StrategyManager::GetDecisionResult(const std::string& decisionId) const {
    auto it = decisions_.find(decisionId);
    return it != decisions_.end() ? &it->second : nullptr;
}

void StrategyManager::AddDelayedConsequence(const DelayedConsequence& consequence) {
    delayedConsequences_.push_back(consequence);
}

void StrategyManager::ProcessDelayedConsequences(int currentTurn) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for (auto& consequence : delayedConsequences_) {
        if (consequence.triggered) continue;
        if (consequence.triggerTurn <= currentTurn) {
            if (dist(rng) < consequence.probability) {
                auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
                if (resourceMgr) {
                    for (const auto& [resource, amount] : consequence.effects) {
                        ResourceType type = ResourceType::PoliticalCapital;
                        if (resource == "Influence") type = ResourceType::Influence;
                        else if (resource == "Information") type = ResourceType::Information;
                        else if (resource == "Wealth") type = ResourceType::Wealth;
                        else if (resource == "Military") type = ResourceType::Military;
                        else if (resource == "Reputation") type = ResourceType::Reputation;
                        else if (resource == "Legitimacy") type = ResourceType::Legitimacy;
                        else if (resource == "PublicSupport") type = ResourceType::PublicSupport;
                        resourceMgr->ModifyResource(type, amount * 0.5f, "Delayed: " + consequence.description);
                    }
                }
            }
            consequence.triggered = true;
        }
    }
}

DecisionTree* StrategyManager::CreateDecisionTree(const std::string& id, const std::string& name) {
    auto tree = std::make_unique<DecisionTree>(id, name);
    auto* ptr = tree.get();
    decisionTrees_[id] = std::move(tree);
    return ptr;
}

DecisionTree* StrategyManager::GetDecisionTree(const std::string& id) {
    auto it = decisionTrees_.find(id);
    return it != decisionTrees_.end() ? it->second.get() : nullptr;
}

float StrategyManager::EvaluateShortTerm(const DecisionOption& option) const {
    return option.shortTermGain;
}

float StrategyManager::EvaluateLongTerm(const DecisionOption& option) const {
    return option.longTermImpact;
}

float StrategyManager::EvaluateRisk(const DecisionOption& option) const {
    return option.risk;
}

float StrategyManager::CalculateOverallScore(const DecisionOption& option) const {
    float shortWeight = 0.3f;
    float longWeight = 0.4f;
    float riskWeight = 0.2f;
    float moralWeight = 0.1f;

    float shortScore = std::max(0.0f, option.shortTermGain) / 100.0f;
    float longScore = std::max(0.0f, option.longTermImpact) / 100.0f;
    float riskScore = 1.0f - (option.risk / 100.0f);
    float moralScore = option.moralWeight;

    return shortScore * shortWeight + longScore * longWeight + riskScore * riskWeight + moralScore * moralWeight;
}

StrategyManager::DecisionAnalysis StrategyManager::AnalyzeDecision(const DecisionOption& option) const {
    DecisionAnalysis analysis;
    analysis.shortTermScore = EvaluateShortTerm(option);
    analysis.longTermScore = EvaluateLongTerm(option);
    analysis.riskScore = EvaluateRisk(option);
    analysis.overallScore = CalculateOverallScore(option);

    if (analysis.overallScore > 0.7f) {
        analysis.recommendation = "Highly recommended - strong overall prospects";
    } else if (analysis.overallScore > 0.5f) {
        analysis.recommendation = "Worth considering - balanced risk/reward";
    } else if (analysis.overallScore > 0.3f) {
        analysis.recommendation = "Proceed with caution - significant risks involved";
    } else {
        analysis.recommendation = "Not recommended - poor risk/reward ratio";
    }

    return analysis;
}

void StrategyManager::ApplyDecisionEffects(const DecisionOption& option) {
    auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
    if (resourceMgr) {
        for (const auto& [resource, amount] : option.resourceEffects) {
            ResourceType type = ResourceType::PoliticalCapital;
            if (resource == "PoliticalCapital") type = ResourceType::PoliticalCapital;
            else if (resource == "Influence") type = ResourceType::Influence;
            else if (resource == "Information") type = ResourceType::Information;
            else if (resource == "Wealth") type = ResourceType::Wealth;
            else if (resource == "Military") type = ResourceType::Military;
            else if (resource == "Reputation") type = ResourceType::Reputation;
            else if (resource == "Legitimacy") type = ResourceType::Legitimacy;
            else if (resource == "PublicSupport") type = ResourceType::PublicSupport;
            resourceMgr->ModifyResource(type, amount, "Decision: " + option.name);
        }
    }
}

void StrategyManager::ResolveDecision(DecisionResult& result) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.8f, 1.2f);

    result.actualOutcome = dist(rng);
    result.resolved = true;
    result.narrative = "Decision resolved with outcome factor: " + std::to_string(result.actualOutcome);
}

}
