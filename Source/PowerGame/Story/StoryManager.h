#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include "../Core/GameSystem.h"
#include "../Core/EventBus.h"

namespace PowerGame {

enum class StoryNodeType {
    MainPlot,
    SideQuest,
    Crisis,
    Opportunity,
    CharacterEvent,
    FactionEvent,
    Climax,
    Ending
};

struct StoryChoice {
    std::string id;
    std::string text;
    std::string description;
    std::vector<std::string> prerequisites;
    std::unordered_map<std::string, float> resourceEffects;
    std::unordered_map<std::string, float> factionEffects;
    std::unordered_map<std::string, float> characterEffects;
    std::string nextNodeId;
    float weight;
};

struct StoryNode {
    std::string id;
    std::string title;
    std::string description;
    StoryNodeType type;
    int triggerTurn;
    std::vector<std::string> triggerConditions;
    std::vector<StoryChoice> choices;
    bool triggered;
    bool completed;
    std::string selectedChoiceId;
    int completedTurn;
};

struct StoryArc {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> nodeIds;
    std::string currentNodeId;
    bool active;
    bool completed;
};

class DynamicStoryGenerator {
public:
    struct GenerationContext {
        int currentTurn;
        std::unordered_map<std::string, float> resources;
        std::string dominantFaction;
        float politicalTension;
        std::vector<std::string> recentEvents;
        std::unordered_map<std::string, float> factionPowers;
    };

    StoryNode GenerateCrisisEvent(const GenerationContext& context);
    StoryNode GenerateOpportunityEvent(const GenerationContext& context);
    StoryNode GenerateCharacterEvent(const GenerationContext& context, const std::string& characterId);
    StoryNode GenerateFactionEvent(const GenerationContext& context, const std::string& factionId);

    float CalculatePoliticalTension(const GenerationContext& context) const;
    bool ShouldTriggerEvent(const GenerationContext& context, float threshold) const;

private:
    std::vector<std::string> GenerateTriggerConditions(StoryNodeType type, const GenerationContext& context);
    std::vector<StoryChoice> GenerateChoices(StoryNodeType type, const GenerationContext& context);
};

class StoryManager : public IGameSystem {
public:
    StoryManager() = default;

    void Initialize() override;
    void Reset() override;
    void OnTurnUpdate(int turn) override;
    void OnFrameUpdate(float deltaTime) override {}
    std::string GetSystemName() const override { return "StoryManager"; }

    StoryNode* AddStoryNode(const StoryNode& node);
    StoryArc* AddStoryArc(const StoryArc& arc);
    const StoryNode* GetStoryNode(const std::string& nodeId) const;
    StoryNode* GetStoryNode(const std::string& nodeId);
    const StoryArc* GetStoryArc(const std::string& arcId) const;
    StoryArc* GetStoryArc(const std::string& arcId);

    bool TriggerStoryNode(const std::string& nodeId);
    bool MakeChoice(const std::string& nodeId, const std::string& choiceId);
    bool CheckTriggerConditions(const std::vector<std::string>& conditions) const;

    std::vector<StoryNode*> GetAvailableNodes() const;
    std::vector<StoryNode*> GetTriggeredNodes() const;
    std::vector<StoryArc*> GetActiveArcs() const;

    DynamicStoryGenerator& GetGenerator() { return generator_; }

    using StoryCallback = std::function<void(const StoryNode&)>;
    using ChoiceCallback = std::function<void(const StoryNode&, const StoryChoice&)>;
    void RegisterStoryCallback(StoryCallback cb) { storyCallbacks_.push_back(cb); }
    void RegisterChoiceCallback(ChoiceCallback cb) { choiceCallbacks_.push_back(cb); }

    const std::vector<std::string>& GetDecisionHistory() const { return decisionHistory_; }

private:
    void InitializeMainStoryline();
    void ProcessDynamicEvents(int turn);
    void ApplyChoiceEffects(const StoryChoice& choice);
    std::string EvaluateCondition(const std::string& condition) const;

    std::unordered_map<std::string, StoryNode> storyNodes_;
    std::unordered_map<std::string, StoryArc> storyArcs_;
    DynamicStoryGenerator generator_;
    std::vector<StoryCallback> storyCallbacks_;
    std::vector<ChoiceCallback> choiceCallbacks_;
    std::vector<std::string> decisionHistory_;
    int nodeIdCounter_ = 0;
};

}
