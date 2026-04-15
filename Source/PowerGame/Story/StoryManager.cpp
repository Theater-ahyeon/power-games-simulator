#include "StoryManager.h"
#include "../Core/GameManager.h"
#include "../Resources/ResourceManager.h"
#include "../Faction/FactionManager.h"
#include <algorithm>
#include <random>

namespace PowerGame {

StoryNode DynamicStoryGenerator::GenerateCrisisEvent(const GenerationContext& context) {
    StoryNode node;
    node.id = "dynamic_crisis_" + std::to_string(context.currentTurn);
    node.type = StoryNodeType::Crisis;
    node.triggerTurn = context.currentTurn;
    node.triggered = false;
    node.completed = false;

    float tension = CalculatePoliticalTension(context);
    if (tension > 70.0f) {
        node.title = "Political Crisis Escalation";
        node.description = "Rising tensions between factions threaten to destabilize the government. "
                          "Multiple parties are demanding immediate action.";
        node.choices = {
            {"suppress", "Suppress opposition forcefully", "Use military/police to maintain order",
             {}, {{"PoliticalCapital", -10}, {"Military", -5}}, {{"radicals", -20}}, {{"lin_zhaoyi", -15}}, "", 1.0f},
            {"negotiate", "Open negotiation channels", "Attempt diplomatic resolution with all parties",
             {}, {{"Influence", -5}}, {{"neutrals", 10}}, {}, "", 1.0f},
            {"manipulate", "Exploit the crisis", "Use the chaos to advance your own agenda",
             {}, {{"PoliticalCapital", 15}, {"Reputation", -10}}, {}, {{"he_shengli", 10}}, "", 0.8f},
            {"wait", "Wait and observe", "Let events unfold before acting",
             {}, {}, {}, {}, "", 0.6f}
        };
    } else {
        node.title = "Emerging Political Tension";
        node.description = "Signs of growing political tension are becoming apparent. "
                          "Early intervention could prevent escalation.";
        node.choices = {
            {"mediate", "Mediate between factions", "Offer to broker peace talks",
             {}, {{"Influence", 5}}, {{"neutrals", 5}}, {}, "", 1.0f},
            {"exploit", "Quietly exploit divisions", "Use the tension to weaken opponents",
             {}, {{"Information", 10}}, {{"radicals", -5}}, {}, "", 0.9f},
            {"strengthen", "Strengthen your position", "Build alliances while others are distracted",
             {}, {{"PoliticalCapital", 5}, {"Wealth", -5}}, {}, {}, "", 1.0f}
        };
    }

    return node;
}

StoryNode DynamicStoryGenerator::GenerateOpportunityEvent(const GenerationContext& context) {
    StoryNode node;
    node.id = "dynamic_opportunity_" + std::to_string(context.currentTurn);
    node.type = StoryNodeType::Opportunity;
    node.triggerTurn = context.currentTurn;
    node.triggered = false;
    node.completed = false;
    node.title = "Unexpected Opportunity";
    node.description = "A unique political opportunity has emerged. Quick action could yield significant advantages.";
    node.choices = {
        {"seize", "Seize the opportunity immediately", "Act quickly to capitalize on the moment",
         {}, {{"PoliticalCapital", 15}, {"Reputation", -5}}, {}, {}, "", 1.0f},
        {"analyze", "Analyze before acting", "Gather more information before committing",
         {}, {{"Information", 10}}, {}, {}, "", 1.0f},
        {"share", "Share with allies", "Include allies to strengthen the outcome",
         {}, {{"Influence", 10}, {"PoliticalCapital", -5}}, {}, {}, "", 0.9f}
    };
    return node;
}

StoryNode DynamicStoryGenerator::GenerateCharacterEvent(const GenerationContext& context,
                                                          const std::string& characterId) {
    StoryNode node;
    node.id = "dynamic_char_" + characterId + "_" + std::to_string(context.currentTurn);
    node.type = StoryNodeType::CharacterEvent;
    node.triggerTurn = context.currentTurn;
    node.triggered = false;
    node.completed = false;
    node.title = "Character Event: " + characterId;
    node.description = "A significant event involving a key political figure has occurred.";
    node.choices = {
        {"support", "Offer support", "Back this character publicly",
         {}, {{"PoliticalCapital", -5}}, {}, {{characterId, 15}}, "", 1.0f},
        {"oppose", "Oppose openly", "Publicly challenge this character",
         {}, {{"Reputation", 5}, {"PoliticalCapital", -10}}, {}, {{characterId, -20}}, "", 0.8f},
        {"covert", "Act covertly", "Work behind the scenes",
         {}, {{"Information", -5}}, {}, {{characterId, -5}}, "", 1.0f}
    };
    return node;
}

StoryNode DynamicStoryGenerator::GenerateFactionEvent(const GenerationContext& context,
                                                        const std::string& factionId) {
    StoryNode node;
    node.id = "dynamic_faction_" + factionId + "_" + std::to_string(context.currentTurn);
    node.type = StoryNodeType::FactionEvent;
    node.triggerTurn = context.currentTurn;
    node.triggered = false;
    node.completed = false;
    node.title = "Faction Event: " + factionId;
    node.description = "A major event within the faction demands attention and response.";
    node.choices = {
        {"align", "Align with faction", "Publicly support the faction's position",
         {}, {{"Influence", 10}}, {{factionId, 15}}, {}, "", 1.0f},
        {"distance", "Distance yourself", "Create separation from the faction",
         {}, {{"PoliticalCapital", 5}}, {{factionId, -10}}, {}, "", 0.9f},
        {"infiltrate", "Infiltrate the faction", "Send agents to gather intelligence",
         {}, {{"Information", 10}, {"Wealth", -5}}, {}, {}, "", 0.7f}
    };
    return node;
}

float DynamicStoryGenerator::CalculatePoliticalTension(const GenerationContext& context) const {
    float tension = 0.0f;
    float maxPower = 0.0f;
    float totalPower = 0.0f;
    for (const auto& [faction, power] : context.factionPowers) {
        totalPower += power;
        maxPower = std::max(maxPower, power);
    }
    if (totalPower > 0) {
        float dominance = maxPower / totalPower;
        tension = (1.0f - dominance) * 50.0f + dominance * 80.0f;
    }
    tension += context.politicalTension * 0.3f;
    return std::clamp(tension, 0.0f, 100.0f);
}

bool DynamicStoryGenerator::ShouldTriggerEvent(const GenerationContext& context, float threshold) const {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 100.0f);
    float tension = CalculatePoliticalTension(context);
    float triggerChance = tension * 0.5f + 20.0f;
    return dist(rng) < triggerChance && triggerChance >= threshold;
}

std::vector<std::string> DynamicStoryGenerator::GenerateTriggerConditions(
    StoryNodeType type, const GenerationContext& context) {
    return {};
}

std::vector<StoryChoice> DynamicStoryGenerator::GenerateChoices(
    StoryNodeType type, const GenerationContext& context) {
    return {};
}

void StoryManager::Initialize() {
    InitializeMainStoryline();
}

void StoryManager::InitializeMainStoryline() {
    std::vector<StoryNode> mainNodes = {
        {"main_01", "The Power Vacuum",
         "The sudden resignation of the Prime Minister has created a power vacuum. "
         "Factions scramble to position themselves for the upcoming succession. "
         "Inspired by real-world leadership transitions and palace coup dynamics.",
         StoryNodeType::MainPlot, 1, {}, {
            {"m1_cooperate", "Build broad coalition", "Seek support across faction lines",
             {}, {{"PoliticalCapital", 10}, {"Influence", 5}}, {{"neutrals", 10}, {"liberals", 5}}, {}, "main_02a", 1.0f},
            {"m1_consolidate", "Consolidate conservative base", "Rally traditional power holders",
             {}, {{"PoliticalCapital", 15}, {"PublicSupport", -5}}, {{"conservatives", 15}, {"radicals", -10}}, {}, "main_02b", 1.0f},
            {"m1_disrupt", "Exploit the chaos", "Use the confusion to advance personal agenda",
             {}, {{"Information", 10}, {"Reputation", -10}}, {}, {{"he_shengli", 10}}, "main_02c", 0.8f},
            {"m1_observe", "Wait and gather intelligence", "Study the situation before committing",
             {}, {{"Information", 15}}, {}, {}, "main_02d", 0.9f},
            {"m1_radical", "Seize the moment radically", "Push for immediate systemic change",
             {}, {{"PoliticalCapital", -5}, {"PublicSupport", 10}}, {{"radicals", 20}, {"conservatives", -15}}, {}, "main_02e", 0.7f}
         }, false, false, "", 0},

        {"main_02a", "Coalition Building",
         "Your coalition-building efforts have attracted attention. Some factions are willing to talk, "
         "while others view your approach with suspicion. The Watergate-style information warfare begins.",
         StoryNodeType::MainPlot, 5, {"main_01:m1_cooperate"}, {
            {"m2a_deepen", "Deepen coalition commitments", "Formalize alliances with written agreements",
             {}, {{"PoliticalCapital", -10}, {"Influence", 15}}, {{"liberals", 10}, {"neutrals", 10}}, {}, "main_03", 1.0f},
            {"m2a_leverage", "Leverage coalition for personal gain", "Use coalition support to extract concessions",
             {}, {{"Wealth", 15}, {"Reputation", -5}}, {}, {{"he_shengli", 5}, {"deng_xiaowei", 5}}, "main_03", 0.8f},
            {"m2a_expand", "Expand coalition to include radicals", "Bring radical elements into the fold",
             {}, {{"PublicSupport", 10}, {"PoliticalCapital", -5}}, {{"radicals", 15}, {"conservatives", -10}}, {}, "main_03", 0.9f}
         }, false, false, "", 0},

        {"main_02b", "Conservative Consolidation",
         "The conservative faction rallies behind your leadership, but at a cost. "
         "Civil liberties are curtailed and opposition grows. Historical parallel: authoritarian consolidation patterns.",
         StoryNodeType::MainPlot, 5, {"main_01:m1_consolidate"}, {
            {"m2b_moderate", "Moderate the approach", "Ease restrictions to placate moderates",
             {}, {{"PublicSupport", 10}, {"PoliticalCapital", -5}}, {{"neutrals", 10}, {"conservatives", -5}}, {}, "main_03", 1.0f},
            {"m2b_harden", "Double down on hardline policies", "Increase control measures",
             {}, {{"Military", 10}, {"PublicSupport", -15}}, {{"conservatives", 10}, {"radicals", -15}}, {}, "main_03", 0.8f},
            {"m2b_purge", "Purge internal dissent", "Remove unreliable elements from the faction",
             {}, {{"PoliticalCapital", 5}, {"Reputation", -10}}, {}, {{"chen_weiming", 10}, {"lin_zhaoyi", -20}}, "main_03", 0.7f}
         }, false, false, "", 0},

        {"main_03", "The Intelligence War",
         "A major intelligence scandal erupts - reminiscent of Watergate's covert operations. "
         "Evidence of systematic surveillance on political opponents surfaces. Your response will define your legacy.",
         StoryNodeType::Crisis, 15, {}, {
            {"m3_expose", "Expose the full truth", "Release all surveillance records publicly",
             {}, {{"Reputation", 15}, {"PoliticalCapital", -20}}, {{"liberals", 15}, {"conservatives", -20}}, {}, "main_04", 1.0f},
            {"m3_cover", "Cover up and deflect", "Deny involvement and redirect blame",
             {}, {{"PoliticalCapital", 10}, {"Integrity", -20}}, {{"conservatives", 5}, {"radicals", -10}}, {}, "main_04", 0.8f},
            {"m3_selective", "Selective disclosure", "Release damaging info about opponents only",
             {}, {{"Information", 15}, {"Reputation", -5}}, {}, {{"deng_xiaowei", 15}}, "main_04", 0.9f},
            {"m3_sacrifice", "Sacrifice subordinates", "Blame lower-level officials for the program",
             {}, {{"PoliticalCapital", -5}, {"Reputation", -10}}, {}, {{"cai_hongyu", -10}}, "main_04", 0.7f}
         }, false, false, "", 0},

        {"main_04", "The Economic Crisis",
         "A severe economic downturn threatens national stability. Banks are failing, unemployment soars. "
         "Your economic decisions will have far-reaching consequences, echoing the 2008 financial crisis response patterns.",
         StoryNodeType::Crisis, 25, {}, {
            {"m4_austerity", "Implement austerity measures", "Cut spending and raise taxes",
             {}, {{"Wealth", -15}, {"PublicSupport", -20}}, {{"conservatives", 10}, {"radicals", -15}}, {}, "main_05", 1.0f},
            {"m4_stimulus", "Massive stimulus package", "Government spending to boost economy",
             {}, {{"Wealth", -25}, {"PublicSupport", 15}}, {{"liberals", 10}, {"conservatives", -10}}, {}, "main_05", 1.0f},
            {"m4_bailout", "Bail out key industries", "Save strategic sectors with public funds",
             {}, {{"Wealth", -20}, {"PoliticalCapital", 10}}, {{"opportunists", 15}}, {{"wang_zhenbang", 15}, {"luo_jiashi", 10}}, "main_05", 0.9f},
            {"m4_letfail", "Let them fail", "Allow market correction without intervention",
             {}, {{"PublicSupport", -25}, {"Reputation", 10}}, {{"radicals", 5}, {"neutrals", -5}}, {}, "main_05", 0.7f}
         }, false, false, "", 0},

        {"main_05", "The Foreign Interference",
         "Evidence emerges of foreign powers attempting to influence domestic politics. "
         "This mirrors real-world election interference cases. The nation's sovereignty is at stake.",
         StoryNodeType::Crisis, 35, {}, {
            {"m5_confront", "Publicly confront foreign power", "Issue strong diplomatic protest",
             {}, {{"Military", 10}, {"Reputation", 15}}, {{"conservatives", 10}, {"radicals", 5}}, {}, "main_06", 1.0f},
            {"m5_covert", "Covert counter-operations", "Secretly undermine foreign influence",
             {}, {{"Information", 15}, {"Military", -5}}, {}, {}, "main_06", 0.9f},
            {"m5_exploit", "Exploit foreign connections", "Use foreign ties for personal advantage",
             {}, {{"Wealth", 20}, {"Legitimacy", -15}}, {{"opportunists", 10}}, {{"wei_donghai", 10}}, "main_06", 0.7f},
            {"m5_negotiate", "Back-channel negotiations", "Seek compromise with foreign power",
             {}, {{"Influence", 10}, {"Reputation", -5}}, {{"neutrals", 5}}, {}, "main_06", 0.8f}
         }, false, false, "", 0},

        {"main_06", "The Constitutional Crisis",
         "A constitutional crisis erupts as the balance of power is challenged. "
         "Parallels to historical constitutional crises and palace coup dynamics emerge. "
         "The very framework of governance hangs in the balance.",
         StoryNodeType::Climax, 50, {}, {
            {"m6_reform", "Push for constitutional reform", "Restructure the political system",
             {}, {{"PoliticalCapital", -20}, {"Legitimacy", 15}}, {{"liberals", 20}, {"conservatives", -15}}, {}, "main_07", 1.0f},
            {"m6_emergency", "Declare emergency powers", "Assume extraordinary authority",
             {}, {{"PoliticalCapital", 20}, {"Legitimacy", -25}, {"Military", 10}}, {{"conservatives", 10}, {"radicals", -20}}, {}, "main_07", 0.8f},
            {"m6_compromise", "Seek grand compromise", "Negotiate power-sharing arrangement",
             {}, {{"Influence", 15}, {"PoliticalCapital", -10}}, {{"neutrals", 15}}, {}, "main_07", 1.0f},
            {"m6_coup", "Support military intervention", "Back a military takeover",
             {}, {{"Military", 25}, {"Legitimacy", -30}, {"PublicSupport", -20}}, {{"radicals", 5}, {"conservatives", -10}}, {{"wu_zhenxiong", 20}, {"feng_daluo", 15}}, "main_07", 0.6f}
         }, false, false, "", 0},

        {"main_07", "The Final Gambit",
         "The endgame approaches. Every decision you've made has led to this moment. "
         "The fate of the nation rests on your final play. Historical echoes of regime transitions resonate.",
         StoryNodeType::Climax, 70, {}, {
            {"m7_consolidate", "Consolidate absolute power", "Eliminate all opposition",
             {}, {{"PoliticalCapital", 30}, {"Reputation", -20}, {"Legitimacy", -15}}, {}, {}, "main_08", 0.8f},
            {"m7_democratize", "Transition to democracy", "Surrender power to democratic process",
             {}, {{"PoliticalCapital", -20}, {"Reputation", 30}, {"Legitimacy", 25}}, {{"liberals", 20}}, {}, "main_08", 1.0f},
            {"m7_balance", "Establish power balance", "Create checks and balances system",
             {}, {{"Influence", 20}, {"PoliticalCapital", -10}}, {{"neutrals", 15}}, {}, "main_08", 1.0f},
            {"m7_shadow", "Retreat to the shadows", "Control from behind the scenes",
             {}, {{"Information", 25}, {"PoliticalCapital", -15}}, {{"opportunists", 10}}, {{"cai_hongyu", 15}, {"he_shengli", 10}}, "main_08", 0.9f}
         }, false, false, "", 0},

        {"main_08", "The New Order",
         "The dust settles. A new political order emerges from the chaos. "
         "Your legacy is being written. The final assessment of your reign begins.",
         StoryNodeType::Ending, 90, {}, {
            {"m8_accept", "Accept the new reality", "Embrace the outcome of your decisions",
             {}, {}, {}, {}, "", 1.0f},
            {"m8_resist", "Resist the outcome", "Fight against the emerging order",
             {}, {{"PoliticalCapital", -10}, {"Military", -5}}, {}, {}, "", 0.5f}
         }, false, false, "", 0}
    };

    for (const auto& node : mainNodes) {
        storyNodes_[node.id] = node;
    }

    StoryArc mainArc;
    mainArc.id = "main_story";
    mainArc.name = "The Struggle for Power";
    mainArc.description = "The main storyline of political intrigue and power struggle";
    mainArc.nodeIds = {"main_01", "main_02a", "main_02b", "main_03", "main_04", "main_05", "main_06", "main_07", "main_08"};
    mainArc.currentNodeId = "main_01";
    mainArc.active = true;
    mainArc.completed = false;
    storyArcs_[mainArc.id] = mainArc;
}

void StoryManager::Reset() {
    for (auto& [id, node] : storyNodes_) {
        node.triggered = false;
        node.completed = false;
        node.selectedChoiceId = "";
        node.completedTurn = 0;
    }
    for (auto& [id, arc] : storyArcs_) {
        arc.active = true;
        arc.completed = false;
        if (!arc.nodeIds.empty()) {
            arc.currentNodeId = arc.nodeIds[0];
        }
    }
    decisionHistory_.clear();
}

void StoryManager::OnTurnUpdate(int turn) {
    ProcessDynamicEvents(turn);

    for (auto& [id, node] : storyNodes_) {
        if (node.triggered || node.completed) continue;
        if (node.triggerTurn > 0 && node.triggerTurn <= turn) {
            if (CheckTriggerConditions(node.triggerConditions)) {
                TriggerStoryNode(id);
            }
        }
    }
}

void StoryManager::ProcessDynamicEvents(int turn) {
    auto& gameMgr = GameManager::Instance();
    auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
    auto* factionMgr = GameSystemManager::Instance().GetSystem<FactionManager>();

    DynamicStoryGenerator::GenerationContext context;
    context.currentTurn = turn;
    if (resourceMgr) context.resources = resourceMgr->GetAllResources();
    if (factionMgr) {
        context.dominantFaction = factionMgr->GetDominantFaction();
        for (const auto& [id, faction] : factionMgr->GetAllFactions()) {
            context.factionPowers[id] = faction->GetPower();
        }
    }
    context.politicalTension = 30.0f;

    if (generator_.ShouldTriggerEvent(context, 50.0f)) {
        float tension = generator_.CalculatePoliticalTension(context);
        StoryNode dynamicNode;
        if (tension > 60.0f) {
            dynamicNode = generator_.GenerateCrisisEvent(context);
        } else {
            dynamicNode = generator_.GenerateOpportunityEvent(context);
        }
        AddStoryNode(dynamicNode);
        TriggerStoryNode(dynamicNode.id);
    }
}

StoryNode* StoryManager::AddStoryNode(const StoryNode& node) {
    auto& inserted = storyNodes_[node.id] = node;
    return &inserted;
}

StoryArc* StoryManager::AddStoryArc(const StoryArc& arc) {
    auto& inserted = storyArcs_[arc.id] = arc;
    return &inserted;
}

const StoryNode* StoryManager::GetStoryNode(const std::string& nodeId) const {
    auto it = storyNodes_.find(nodeId);
    return it != storyNodes_.end() ? &it->second : nullptr;
}

StoryNode* StoryManager::GetStoryNode(const std::string& nodeId) {
    auto it = storyNodes_.find(nodeId);
    return it != storyNodes_.end() ? &it->second : nullptr;
}

const StoryArc* StoryManager::GetStoryArc(const std::string& arcId) const {
    auto it = storyArcs_.find(arcId);
    return it != storyArcs_.end() ? &it->second : nullptr;
}

StoryArc* StoryManager::GetStoryArc(const std::string& arcId) {
    auto it = storyArcs_.find(arcId);
    return it != storyArcs_.end() ? &it->second : nullptr;
}

bool StoryManager::TriggerStoryNode(const std::string& nodeId) {
    auto it = storyNodes_.find(nodeId);
    if (it == storyNodes_.end()) return false;
    if (it->second.triggered) return false;

    it->second.triggered = true;

    EventBus::Instance().Publish(Events::StoryEventTriggered, StoryEvent{nodeId, "", 0});
    for (auto& cb : storyCallbacks_) {
        cb(it->second);
    }

    return true;
}

bool StoryManager::MakeChoice(const std::string& nodeId, const std::string& choiceId) {
    auto it = storyNodes_.find(nodeId);
    if (it == storyNodes_.end()) return false;
    if (!it->second.triggered || it->second.completed) return false;

    auto& node = it->second;
    auto choiceIt = std::find_if(node.choices.begin(), node.choices.end(),
        [&choiceId](const StoryChoice& c) { return c.id == choiceId; });
    if (choiceIt == node.choices.end()) return false;

    node.selectedChoiceId = choiceId;
    node.completed = true;
    node.completedTurn = GameManager::Instance().GetCurrentTurn();

    decisionHistory_.push_back(nodeId + ":" + choiceId);

    ApplyChoiceEffects(*choiceIt);

    EventBus::Instance().Publish(Events::StoryChoiceMade, StoryEvent{nodeId, choiceId, node.completedTurn});
    for (auto& cb : choiceCallbacks_) {
        cb(node, *choiceIt);
    }

    if (!choiceIt->nextNodeId.empty()) {
        auto nextIt = storyNodes_.find(choiceIt->nextNodeId);
        if (nextIt != storyNodes_.end() && !nextIt->second.triggered) {
            TriggerStoryNode(choiceIt->nextNodeId);
        }
    }

    return true;
}

bool StoryManager::CheckTriggerConditions(const std::vector<std::string>& conditions) const {
    for (const auto& condition : conditions) {
        if (EvaluateCondition(condition) != "true") return false;
    }
    return true;
}

std::string StoryManager::EvaluateCondition(const std::string& condition) const {
    size_t colonPos = condition.find(':');
    if (colonPos == std::string::npos) return "true";

    std::string nodeId = condition.substr(0, colonPos);
    std::string requiredChoice = condition.substr(colonPos + 1);

    std::string key = nodeId + ":" + requiredChoice;
    for (const auto& decision : decisionHistory_) {
        if (decision == key) return "true";
    }
    return "false";
}

std::vector<StoryNode*> StoryManager::GetAvailableNodes() const {
    std::vector<StoryNode*> result;
    for (auto& [id, node] : storyNodes_) {
        if (node.triggered && !node.completed) {
            result.push_back(const_cast<StoryNode*>(&node));
        }
    }
    return result;
}

std::vector<StoryNode*> StoryManager::GetTriggeredNodes() const {
    std::vector<StoryNode*> result;
    for (auto& [id, node] : storyNodes_) {
        if (node.triggered) {
            result.push_back(const_cast<StoryNode*>(&node));
        }
    }
    return result;
}

std::vector<StoryArc*> StoryManager::GetActiveArcs() const {
    std::vector<StoryArc*> result;
    for (auto& [id, arc] : storyArcs_) {
        if (arc.active && !arc.completed) {
            result.push_back(const_cast<StoryArc*>(&arc));
        }
    }
    return result;
}

void StoryManager::ApplyChoiceEffects(const StoryChoice& choice) {
    auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
    if (resourceMgr) {
        for (const auto& [resource, amount] : choice.resourceEffects) {
            ResourceType type = ResourceType::PoliticalCapital;
            if (resource == "PoliticalCapital") type = ResourceType::PoliticalCapital;
            else if (resource == "Influence") type = ResourceType::Influence;
            else if (resource == "Information") type = ResourceType::Information;
            else if (resource == "Wealth") type = ResourceType::Wealth;
            else if (resource == "Military") type = ResourceType::Military;
            else if (resource == "Reputation") type = ResourceType::Reputation;
            else if (resource == "Legitimacy") type = ResourceType::Legitimacy;
            else if (resource == "PublicSupport") type = ResourceType::PublicSupport;
            resourceMgr->ModifyResource(type, amount, "Story choice: " + choice.id);
        }
    }
}

}
