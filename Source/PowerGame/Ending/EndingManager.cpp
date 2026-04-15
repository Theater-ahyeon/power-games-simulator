#include "EndingManager.h"
#include "../Core/GameManager.h"
#include "../Resources/ResourceManager.h"
#include "../Faction/FactionManager.h"
#include "../Story/StoryManager.h"
#include "../Strategy/StrategyManager.h"
#include <algorithm>
#include <cmath>

namespace PowerGame {

void EndingManager::Initialize() {
    InitializeEndings();
    InitializeConditions();
}

void EndingManager::InitializeEndings() {
    endings_[EndingType::Dictatorship] = {
        EndingType::Dictatorship, "ending_dictatorship",
        "The Iron Throne",
        "Through ruthless consolidation of power, you have established an unbreakable grip on the nation. "
        "Opposition has been silenced, institutions bent to your will, and the military stands firmly behind you. "
        "The people whisper in fear, but the trains run on time.",
        "Your reign will be remembered as one of absolute authority. History will debate whether stability "
        "was worth the price of freedom. The shadows of your purges will haunt generations to come.",
        {}, 0.0f, false
    };

    endings_[EndingType::DemocraticReform] = {
        EndingType::DemocraticReform, "ending_democracy",
        "The Dawn of Democracy",
        "Against all odds, you have steered the nation toward democratic reform. Free elections have been "
        "established, civil liberties protected, and power peacefully transferred. Your legacy as a reformer "
        "is secured, though the path was fraught with sacrifice.",
        "The democratic institutions you built will be tested by time. Some question whether the people "
        "are ready for true freedom, but you have given them the chance to prove the skeptics wrong.",
        {}, 0.0f, false
    };

    endings_[EndingType::PowerBalance] = {
        EndingType::PowerBalance, "ending_balance",
        "The Grand Equilibrium",
        "You have achieved what few thought possible: a stable balance of power between all factions. "
        "No single group dominates, and compromise has become the currency of politics. The system holds, "
        "but tensions simmer beneath the surface.",
        "The balance you struck is delicate. Future leaders will need the same wisdom to maintain it. "
        "For now, the nation enjoys an era of relative peace and prosperity.",
        {}, 0.0f, false
    };

    endings_[EndingType::ExileFailure] = {
        EndingType::ExileFailure, "ending_exile",
        "The Fallen Leader",
        "Your ambitions exceeded your grasp. Political enemies have united against you, and you find yourself "
        "stripped of power, wealth, and influence. Whether through betrayal, miscalculation, or simply "
        "bad luck, your political career has ended in disgrace.",
        "Exile is bitter, but not all is lost. History is full of comebacks. Whether you choose to fight "
        "another day or accept your fate, the lessons of your fall will not be forgotten.",
        {}, 0.0f, false
    };

    endings_[EndingType::ShadowControl] = {
        EndingType::ShadowControl, "ending_shadow",
        "The Puppet Master",
        "While others hold the visible reins of power, you control from the shadows. Your network of "
        "informants, blackmail material, and strategic alliances gives you influence that transcends "
        "any official position. The public face of power changes, but you remain the constant.",
        "Your shadow empire will endure as long as your secrets remain safe and your agents stay loyal. "
        "But in the world of intrigue, there is always someone plotting in the dark.",
        {}, 0.0f, false
    };

    endings_[EndingType::SystemCollapse] = {
        EndingType::SystemCollapse, "ending_collapse",
        "The House of Cards",
        "The political system has collapsed under the weight of its own contradictions. Factions have "
        "turned on each other, institutions have failed, and chaos reigns. The nation teeters on the "
        "brink of civil war or dissolution.",
        "From the ashes, something new may rise. Whether it will be better or worse than what came before "
        "depends on who seizes the moment. The people suffer, but they are resilient.",
        {}, 0.0f, false
    };

    endings_[EndingType::ForeignIntervention] = {
        EndingType::ForeignIntervention, "ending_foreign",
        "The External Hand",
        "Foreign powers have decisively intervened in domestic affairs, reshaping the political landscape "
        "to serve their interests. Whether through economic pressure, diplomatic manipulation, or covert "
        "operations, the nation's sovereignty has been compromised.",
        "The question of national independence will define the coming generation. Some will resist, "
        "others will collaborate. The struggle for true self-determination is far from over.",
        {}, 0.0f, false
    };

    endings_[EndingType::PersonalAmbition] = {
        EndingType::PersonalAmbition, "ending_ambition",
        "The Self-Made Sovereign",
        "Your personal ambition has been fulfilled beyond your wildest dreams. Wealth, power, and influence "
        "are yours in abundance. You have risen from nothing to become the most powerful individual in the nation, "
        "answering to no one.",
        "But at what cost? The friends you've lost, the principles you've abandoned, the trust you've broken - "
        "these weigh heavy on the soul. Was it worth it? Only you can answer that.",
        {}, 0.0f, false
    };
}

void EndingManager::InitializeConditions() {
    conditions_ = {
        {"cond_dictatorship_1", "Absolute power consolidation", EndingType::Dictatorship,
         {{"PoliticalCapital", 80}, {"Military", 60}, {"Legitimacy", 20}},
         {{"conservatives", 60}},
         {"main_06:m6_emergency", "main_07:m7_consolidate"},
         {"main_06:m6_reform", "main_07:m7_democratize"},
         90.0f},

        {"cond_dictatorship_2", "Military-backed dictatorship", EndingType::Dictatorship,
         {{"Military", 80}, {"PoliticalCapital", 50}},
         {{"radicals", 40}},
         {"main_06:m6_coup"},
         {"main_07:m7_democratize"},
         85.0f},

        {"cond_democracy_1", "Democratic transition", EndingType::DemocraticReform,
         {{"Legitimacy", 70}, {"PublicSupport", 60}, {"Reputation", 60}},
         {{"liberals", 50}},
         {"main_06:m6_reform", "main_07:m7_democratize"},
         {"main_06:m6_emergency", "main_07:m7_consolidate"},
         88.0f},

        {"cond_democracy_2", "Peaceful reform path", EndingType::DemocraticReform,
         {{"PublicSupport", 70}, {"Reputation", 70}},
         {{"neutrals", 40}, {"liberals", 40}},
         {"main_01:m1_cooperate", "main_07:m7_democratize"},
         {"main_06:m6_coup"},
         85.0f},

        {"cond_balance_1", "Power equilibrium", EndingType::PowerBalance,
         {{"Influence", 60}, {"PoliticalCapital", 40}},
         {},
         {"main_06:m6_compromise", "main_07:m7_balance"},
         {"main_07:m7_consolidate", "main_07:m7_democratize"},
         80.0f},

        {"cond_balance_2", "Negotiated settlement", EndingType::PowerBalance,
         {{"Influence", 50}, {"Reputation", 50}},
         {{"neutrals", 50}},
         {"main_01:m1_cooperate", "main_05:m5_negotiate"},
         {},
         75.0f},

        {"cond_exile_1", "Political downfall", EndingType::ExileFailure,
         {{"PoliticalCapital", 10}, {"Reputation", 10}},
         {},
         {},
         {},
         70.0f},

        {"cond_exile_2", "Betrayed and exiled", EndingType::ExileFailure,
         {{"Legitimacy", 10}},
         {},
         {},
         {},
         65.0f},

        {"cond_shadow_1", "Shadow control established", EndingType::ShadowControl,
         {{"Information", 80}, {"Influence", 60}},
         {{"opportunists", 50}},
         {"main_07:m7_shadow"},
         {"main_07:m7_consolidate"},
         82.0f},

        {"cond_shadow_2", "Intelligence network dominance", EndingType::ShadowControl,
         {{"Information", 70}, {"Wealth", 50}},
         {},
         {"main_01:m1_observe", "main_03:m3_selective"},
         {},
         78.0f},

        {"cond_collapse_1", "System failure", EndingType::SystemCollapse,
         {{"Legitimacy", 15}, {"PublicSupport", 15}},
         {},
         {"main_04:m4_letfail"},
         {},
         72.0f},

        {"cond_collapse_2", "Factional civil war", EndingType::SystemCollapse,
         {},
         {{"conservatives", 80}, {"radicals", 80}},
         {"main_06:m6_coup"},
         {"main_07:m7_balance"},
         75.0f},

        {"cond_foreign_1", "Foreign domination", EndingType::ForeignIntervention,
         {{"Legitimacy", 20}, {"Military", 20}},
         {},
         {"main_05:m5_exploit"},
         {"main_05:m5_confront"},
         68.0f},

        {"cond_foreign_2", "Economic dependency", EndingType::ForeignIntervention,
         {{"Wealth", 15}, {"Legitimacy", 25}},
         {},
         {"main_05:m5_negotiate", "main_04:m4_bailout"},
         {},
         65.0f},

        {"cond_ambition_1", "Personal empire", EndingType::PersonalAmbition,
         {{"Wealth", 80}, {"PoliticalCapital", 60}, {"Influence", 60}},
         {},
         {"main_01:m1_disrupt", "main_07:m7_shadow"},
         {"main_07:m7_democratize"},
         77.0f},

        {"cond_ambition_2", "Kingmaker achievement", EndingType::PersonalAmbition,
         {{"Wealth", 70}, {"Information", 70}},
         {{"opportunists", 60}},
         {"main_03:m3_manipulate", "main_07:m7_shadow"},
         {},
         73.0f},
    };
}

void EndingManager::Reset() {
    for (auto& [type, ending] : endings_) {
        ending.score = 0.0f;
        ending.unlocked = false;
        ending.triggeredBy.clear();
    }
    triggeredEnding_ = std::nullopt;
}

void EndingManager::OnTurnUpdate(int turn) {
}

bool EndingManager::CheckEndings() {
    auto evaluations = EvaluateAllEndings();
    if (evaluations.empty()) return false;

    std::sort(evaluations.begin(), evaluations.end(),
        [](const Ending& a, const Ending& b) { return a.score > b.score; });

    if (evaluations[0].score >= 60.0f) {
        triggeredEnding_ = evaluations[0];
        for (auto& cb : endingCallbacks_) {
            cb(*triggeredEnding_);
        }
        EventBus::Instance().Publish(Events::EndingTriggered, EndingEvent{
            triggeredEnding_->id, triggeredEnding_->title, triggeredEnding_->triggeredBy
        });
        return true;
    }

    return false;
}

std::vector<Ending> EndingManager::EvaluateAllEndings() const {
    std::vector<Ending> results;

    for (const auto& [type, ending] : endings_) {
        float maxScore = 0.0f;
        std::vector<std::string> triggeredBy;

        for (const auto& condition : conditions_) {
            if (condition.type != type) continue;
            if (EvaluateCondition(condition)) {
                float score = CalculateEndingScore(type);
                if (score > maxScore) {
                    maxScore = score;
                    triggeredBy.push_back(condition.id);
                }
            }
        }

        if (maxScore > 0) {
            Ending evaluated = ending;
            evaluated.score = maxScore;
            evaluated.triggeredBy = triggeredBy;
            evaluated.unlocked = true;
            results.push_back(evaluated);
        }
    }

    return results;
}

void EndingManager::AddEndingCondition(const EndingCondition& condition) {
    conditions_.push_back(condition);
}

float EndingManager::CalculateEndingScore(EndingType type) const {
    float totalScore = 0.0f;
    int conditionCount = 0;

    for (const auto& condition : conditions_) {
        if (condition.type != type) continue;
        conditionCount++;

        float resourceMatch = CalculateResourceMatch(condition.resourceRequirements);
        float factionMatch = CalculateFactionMatch(condition.factionRequirements);
        bool decisionMatch = CheckDecisionHistory(condition.requiredDecisions, condition.blockedDecisions);

        float conditionScore = resourceMatch * 0.35f + factionMatch * 0.25f + (decisionMatch ? 1.0f : 0.0f) * 0.4f;
        conditionScore *= condition.priority / 100.0f;

        totalScore = std::max(totalScore, conditionScore);
    }

    return totalScore * 100.0f;
}

bool EndingManager::IsEndingUnlocked(EndingType type) const {
    for (const auto& condition : conditions_) {
        if (condition.type == type && EvaluateCondition(condition)) {
            return true;
        }
    }
    return false;
}

bool EndingManager::EvaluateCondition(const EndingCondition& condition) const {
    float resourceMatch = CalculateResourceMatch(condition.resourceRequirements);
    if (resourceMatch < 0.5f) return false;

    float factionMatch = CalculateFactionMatch(condition.factionRequirements);
    if (factionMatch < 0.5f && !condition.factionRequirements.empty()) return false;

    return CheckDecisionHistory(condition.requiredDecisions, condition.blockedDecisions);
}

float EndingManager::CalculateResourceMatch(const std::unordered_map<std::string, float>& requirements) const {
    if (requirements.empty()) return 1.0f;

    auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
    if (!resourceMgr) return 0.0f;

    float totalMatch = 0.0f;
    int count = 0;

    for (const auto& [resource, required] : requirements) {
        ResourceType type = ResourceType::PoliticalCapital;
        if (resource == "PoliticalCapital") type = ResourceType::PoliticalCapital;
        else if (resource == "Influence") type = ResourceType::Influence;
        else if (resource == "Information") type = ResourceType::Information;
        else if (resource == "Wealth") type = ResourceType::Wealth;
        else if (resource == "Military") type = ResourceType::Military;
        else if (resource == "Reputation") type = ResourceType::Reputation;
        else if (resource == "Legitimacy") type = ResourceType::Legitimacy;
        else if (resource == "PublicSupport") type = ResourceType::PublicSupport;

        float current = resourceMgr->GetResource(type);
        float match = std::min(current / required, 1.0f);
        totalMatch += match;
        count++;
    }

    return count > 0 ? totalMatch / count : 1.0f;
}

float EndingManager::CalculateFactionMatch(const std::unordered_map<std::string, float>& requirements) const {
    if (requirements.empty()) return 1.0f;

    auto* factionMgr = GameSystemManager::Instance().GetSystem<FactionManager>();
    if (!factionMgr) return 0.0f;

    float totalMatch = 0.0f;
    int count = 0;

    for (const auto& [faction, required] : requirements) {
        auto* faction = factionMgr->GetFaction(faction);
        float current = faction ? faction->GetPower() : 0.0f;
        float match = std::min(current / required, 1.0f);
        totalMatch += match;
        count++;
    }

    return count > 0 ? totalMatch / count : 1.0f;
}

bool EndingManager::CheckDecisionHistory(const std::vector<std::string>& required,
                                            const std::vector<std::string>& blocked) const {
    auto* storyMgr = GameSystemManager::Instance().GetSystem<StoryManager>();
    if (!storyMgr) return required.empty();

    const auto& history = storyMgr->GetDecisionHistory();

    for (const auto& req : required) {
        bool found = false;
        for (const auto& decision : history) {
            if (decision == req) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }

    for (const auto& block : blocked) {
        for (const auto& decision : history) {
            if (decision == block) return false;
        }
    }

    return true;
}

}
