#include "AIDecisionEngine.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace PowerGame {

std::string UtilityAI::Evaluate(const std::vector<Action>& actions) {
    if (actions.empty()) return "";

    float bestScore = -1.0f;
    std::string bestAction;

    for (const auto& action : actions) {
        float score = CalculateScore(action);
        if (score > bestScore) {
            bestScore = score;
            bestAction = action.name;
        }
    }

    return bestAction;
}

float UtilityAI::CalculateScore(const Action& action) {
    float combined = CombineConsiderations(action.considerations);
    return combined + action.bonus;
}

float UtilityAI::CombineConsiderations(const std::vector<Consideration>& considerations) {
    if (considerations.empty()) return 0.0f;

    float result = 1.0f;
    float totalWeight = 0.0f;

    for (const auto& con : considerations) {
        float input = con.input();
        float evaluated = con.curve(input);
        result *= evaluated * con.weight + (1.0f - con.weight);
        totalWeight += con.weight;
    }

    if (totalWeight > 0) {
        result = std::pow(result, 1.0f / totalWeight);
    }

    return std::clamp(result, 0.0f, 1.0f);
}

AIDecisionEngine::AIAction AIDecisionEngine::Decide(const DecisionContext& context) {
    auto ranked = RankActions(context);
    if (ranked.empty()) {
        return {"wait", 0.5f, "No viable actions available"};
    }
    return ranked[0];
}

std::vector<AIDecisionEngine::AIAction> AIDecisionEngine::RankActions(const DecisionContext& context) {
    std::vector<AIAction> actions;

    for (const auto& tmpl : actionTemplates_) {
        float utility = tmpl.utilityFunc(context);
        actions.push_back({tmpl.name, utility, tmpl.description});
    }

    actions.push_back({"gather_intelligence", context.resources.count("Information") > 0 ?
        (1.0f - context.resources.at("Information") / 100.0f) * 0.7f : 0.3f,
        "Intelligence is crucial for informed decisions"});

    actions.push_back({"build_alliance", context.politicalTension > 50.0f ? 0.6f : 0.3f,
        "Alliances provide stability in turbulent times"});

    actions.push_back({"consolidate_power", context.personalThreat > 60.0f ? 0.7f : 0.4f,
        "Power consolidation reduces vulnerability"});

    actions.push_back({"economic_investment", context.resources.count("Wealth") > 0 ?
        (context.resources.at("Wealth") / 100.0f) * 0.5f : 0.2f,
        "Economic strength underpins political power"});

    actions.push_back({"public_campaign", context.resources.count("PublicSupport") > 0 ?
        (1.0f - context.resources.at("PublicSupport") / 100.0f) * 0.5f : 0.3f,
        "Public support is the foundation of legitimacy"});

    actions.push_back({"covert_operation", context.politicalTension > 70.0f ? 0.5f : 0.2f,
        "Covert actions can shift the balance secretly"});

    actions.push_back({"wait_observe", 0.3f,
        "Patience can reveal opportunities"});

    std::sort(actions.begin(), actions.end(),
        [](const AIAction& a, const AIAction& b) { return a.utility > b.utility; });

    return actions;
}

void AIDecisionEngine::RegisterActionTemplate(const std::string& name,
                                                 std::function<float(const DecisionContext&)> utilityFunc,
                                                 const std::string& description) {
    actionTemplates_.push_back({name, utilityFunc, description});
}

}
