#include <iostream>
#include <cassert>
#include <cmath>
#include <string>
#include <vector>
#include <unordered_map>

#include "../Source/PowerGame/Core/GameSystem.h"
#include "../Source/PowerGame/Core/EventBus.h"
#include "../Source/PowerGame/Core/GameManager.h"
#include "../Source/PowerGame/Resources/ResourceManager.h"
#include "../Source/PowerGame/Faction/FactionManager.h"
#include "../Source/PowerGame/Intelligence/IntelligenceManager.h"
#include "../Source/PowerGame/Alliance/AllianceManager.h"
#include "../Source/PowerGame/Character/CharacterManager.h"
#include "../Source/PowerGame/Story/StoryManager.h"
#include "../Source/PowerGame/Strategy/StrategyManager.h"
#include "../Source/PowerGame/Ending/EndingManager.h"
#include "../Source/PowerGame/AI/AIDecisionEngine.h"

using namespace PowerGame;

void TestResourceManager() {
    std::cout << "Testing ResourceManager..." << std::endl;
    ResourceManager mgr;
    mgr.Initialize();

    assert(mgr.GetResource(ResourceType::PoliticalCapital) == 50.0f);
    assert(mgr.GetResource(ResourceType::Influence) == 30.0f);
    assert(mgr.GetResource(ResourceType::Information) == 20.0f);

    assert(mgr.ModifyResource(ResourceType::PoliticalCapital, 10.0f, "test"));
    assert(std::abs(mgr.GetResource(ResourceType::PoliticalCapital) - 60.0f) < 0.01f);

    assert(!mgr.ModifyResource(ResourceType::PoliticalCapital, -200.0f, "test"));

    assert(mgr.ConvertResource(ResourceType::Wealth, ResourceType::PoliticalCapital, 5.0f));

    mgr.AddDelayedEffect(ResourceType::Influence, 10.0f, 3, "delayed test");
    mgr.OnTurnUpdate(1);
    mgr.OnTurnUpdate(2);
    mgr.OnTurnUpdate(3);

    auto all = mgr.GetAllResources();
    assert(all.size() == 8);

    std::cout << "  ResourceManager tests PASSED" << std::endl;
}

void TestFactionManager() {
    std::cout << "Testing FactionManager..." << std::endl;
    FactionManager mgr;
    mgr.Initialize();

    auto* faction = mgr.GetFaction("conservatives");
    assert(faction != nullptr);
    assert(faction->GetName() == "Conservative Alliance");

    auto* libs = mgr.GetFaction("liberals");
    assert(libs != nullptr);

    FactionMember member{"char_001", 70.0f, 50.0f, "advisor"};
    faction->AddMember(member);
    assert(faction->GetMemberCount() == 1);

    faction->ModifyPower(20.0f);
    assert(std::abs(faction->GetPower() - 30.0f) < 0.01f);

    float balance = mgr.GetFactionBalance();
    assert(balance >= 0.0f && balance <= 1.0f);

    std::string dominant = mgr.GetDominantFaction();
    assert(!dominant.empty());

    auto& loyalty = mgr.GetLoyaltySystem();
    loyalty.UpdateLoyalty("char_001", "conservatives", 10.0f);
    assert(loyalty.GetLoyalty("char_001", "conservatives") == 60.0f);

    std::cout << "  FactionManager tests PASSED" << std::endl;
}

void TestIntelligenceManager() {
    std::cout << "Testing IntelligenceManager..." << std::endl;
    IntelligenceManager mgr;
    mgr.Initialize();

    auto* spy = mgr.DeploySpy("Agent Alpha", "conservatives", "liberals", 70.0f);
    assert(spy != nullptr);
    assert(spy->active);
    assert(spy->skill == 70.0f);

    auto* network = mgr.CreateNetwork("Shadow Net", "liberals");
    assert(network != nullptr);

    mgr.AddAgentToNetwork(network->id, spy->id);

    auto intel = mgr.GatherIntel(spy->id, IntelType::PoliticalSecret, "conservatives");
    assert(!intel.id.empty());

    float security = mgr.GetSecurityLevel("conservatives");
    assert(security > 0.0f);

    std::cout << "  IntelligenceManager tests PASSED" << std::endl;
}

void TestAllianceManager() {
    std::cout << "Testing AllianceManager..." << std::endl;
    AllianceManager mgr;
    mgr.Initialize();

    auto* treaty = mgr.ProposeTreaty(AllianceType::PoliticalCoalition,
        "conservatives", "neutrals", {}, 20);
    assert(treaty != nullptr);
    assert(treaty->GetStatus() == TreatyStatus::Proposed);

    assert(mgr.AcceptTreaty(treaty->GetId()));
    assert(treaty->GetStatus() == TreatyStatus::Active);

    float trust = mgr.GetFactionTrustLevel("conservatives", "neutrals");
    assert(trust > 50.0f);

    auto eval = mgr.EvaluateTrust("conservatives", "neutrals");
    assert(eval.trustScore > 0.0f);

    auto activeTreaties = mgr.GetActiveTreatiesForFaction("conservatives");
    assert(!activeTreaties.empty());

    std::cout << "  AllianceManager tests PASSED" << std::endl;
}

void TestCharacterManager() {
    std::cout << "Testing CharacterManager..." << std::endl;
    CharacterManager mgr;
    mgr.Initialize();

    auto characters = mgr.GetActiveCharacters();
    assert(characters.size() >= 20);

    auto* chen = mgr.GetCharacter("chen_weiming");
    assert(chen != nullptr);
    assert(chen->GetStance() == PoliticalStance::Conservative);

    const auto& attrs = chen->GetAttributes();
    assert(attrs.scheming == 85.0f);
    assert(attrs.wealth == 90.0f);

    float powerScore = attrs.GetPowerScore();
    assert(powerScore > 0.0f);

    float trustScore = attrs.GetTrustScore();
    assert(trustScore >= 0.0f && trustScore <= 100.0f);

    chen->ModifyFavorability("zhang_haoran", 15.0f);
    assert(std::abs(chen->GetFavorability("zhang_haoran") - 65.0f) < 0.01f);

    auto conservatives = mgr.GetCharactersByStance(PoliticalStance::Conservative);
    assert(conservatives.size() >= 4);

    auto decision = mgr.GetCharacterDecision("chen_weiming", 1);
    assert(!decision.reasoning.empty());

    std::cout << "  CharacterManager tests PASSED" << std::endl;
}

void TestStoryManager() {
    std::cout << "Testing StoryManager..." << std::endl;
    StoryManager mgr;
    mgr.Initialize();

    auto* node = mgr.GetStoryNode("main_01");
    assert(node != nullptr);
    assert(node->title == "The Power Vacuum");
    assert(node->choices.size() >= 3);

    assert(mgr.TriggerStoryNode("main_01"));
    assert(node->triggered);

    auto available = mgr.GetAvailableNodes();
    assert(!available.empty());

    assert(mgr.MakeChoice("main_01", "m1_cooperate"));
    assert(node->completed);
    assert(node->selectedChoiceId == "m1_cooperate");

    auto& history = mgr.GetDecisionHistory();
    assert(!history.empty());

    std::cout << "  StoryManager tests PASSED" << std::endl;
}

void TestStrategyManager() {
    std::cout << "Testing StrategyManager..." << std::endl;
    StrategyManager mgr;
    mgr.Initialize();

    auto* tree = mgr.GetDecisionTree("main_strategy");
    assert(tree != nullptr);
    assert(tree->GetMaxDepth() >= 3);

    std::vector<DecisionOption> options = {
        {"opt_a", "Option A", "Test A", 20.0f, 10.0f, 30.0f, {}, {}, {}, 0.5f},
        {"opt_b", "Option B", "Test B", -5.0f, 25.0f, 50.0f, {}, {}, {}, 0.7f},
        {"opt_c", "Option C", "Test C", 10.0f, 5.0f, 10.0f, {}, {}, {}, 0.6f}
    };

    std::string decisionId = mgr.CreateDecision("Test Decision", "Testing", options);
    assert(!decisionId.empty());

    auto analysis = mgr.AnalyzeDecision(options[0]);
    assert(analysis.overallScore > 0.0f);
    assert(!analysis.recommendation.empty());

    float shortScore = mgr.EvaluateShortTerm(options[0]);
    assert(shortScore == 20.0f);

    float longScore = mgr.EvaluateLongTerm(options[1]);
    assert(longScore == 25.0f);

    std::cout << "  StrategyManager tests PASSED" << std::endl;
}

void TestEndingManager() {
    std::cout << "Testing EndingManager..." << std::endl;
    EndingManager mgr;
    mgr.Initialize();

    auto& endings = mgr.GetAllEndings();
    assert(endings.size() == 8);

    auto& conditions = mgr.GetConditions();
    assert(conditions.size() >= 15);

    auto evaluations = mgr.EvaluateAllEndings();
    assert(!evaluations.empty());

    for (const auto& ending : evaluations) {
        assert(ending.score >= 0.0f);
        assert(!ending.title.empty());
    }

    std::cout << "  EndingManager tests PASSED" << std::endl;
}

void TestAIDecisionEngine() {
    std::cout << "Testing AIDecisionEngine..." << std::endl;
    AIDecisionEngine engine;

    AIDecisionEngine::DecisionContext context;
    context.characterId = "test_char";
    context.turn = 10;
    context.resources = {{"PoliticalCapital", 50}, {"Influence", 40}, {"Information", 30}};
    context.factionPowers = {{"conservatives", 35}, {"liberals", 25}};
    context.politicalTension = 45.0f;
    context.personalThreat = 20.0f;

    auto decision = engine.Decide(context);
    assert(!decision.name.empty());
    assert(decision.utility >= 0.0f);

    auto ranked = engine.RankActions(context);
    assert(!ranked.empty());
    for (size_t i = 1; i < ranked.size(); i++) {
        assert(ranked[i-1].utility >= ranked[i].utility);
    }

    std::cout << "  AIDecisionEngine tests PASSED" << std::endl;
}

void TestEventBus() {
    std::cout << "Testing EventBus..." << std::endl;
    auto& bus = EventBus::Instance();
    bus.Clear();

    bool received = false;
    ResourceChangeEvent receivedEvent;
    bus.Subscribe<ResourceChangeEvent>(Events::ResourceChanged,
        [&received, &receivedEvent](const ResourceChangeEvent& evt) {
            received = true;
            receivedEvent = evt;
        });

    ResourceChangeEvent testEvent;
    testEvent.type = ResourceChangeEvent::PoliticalCapital;
    testEvent.oldValue = 50.0f;
    testEvent.newValue = 60.0f;
    testEvent.reason = "test";
    bus.Publish(Events::ResourceChanged, testEvent);

    assert(received);
    assert(receivedEvent.newValue == 60.0f);

    bus.Clear();
    std::cout << "  EventBus tests PASSED" << std::endl;
}

void TestBehaviorTree() {
    std::cout << "Testing BehaviorTree..." << std::endl;

    using namespace BehaviorTree;

    auto sequence = std::make_unique<Sequence>();
    bool action1Called = false;
    bool action2Called = false;

    sequence->AddChild(std::make_unique<Action>([&action1Called]() {
        action1Called = true;
        return NodeStatus::Success;
    }));
    sequence->AddChild(std::make_unique<Action>([&action2Called]() {
        action2Called = true;
        return NodeStatus::Success;
    }));

    auto result = sequence->Execute();
    assert(result == NodeStatus::Success);
    assert(action1Called);
    assert(action2Called);

    auto selector = std::make_unique<Selector>();
    bool failActionCalled = false;
    bool successActionCalled = false;

    selector->AddChild(std::make_unique<Action>([&failActionCalled]() {
        failActionCalled = true;
        return NodeStatus::Failure;
    }));
    selector->AddChild(std::make_unique<Action>([&successActionCalled]() {
        successActionCalled = true;
        return NodeStatus::Success;
    }));

    result = selector->Execute();
    assert(result == NodeStatus::Success);
    assert(failActionCalled);
    assert(successActionCalled);

    auto inverter = std::make_unique<Inverter>(std::make_unique<Action>([]() {
        return NodeStatus::Success;
    }));
    result = inverter->Execute();
    assert(result == NodeStatus::Failure);

    std::cout << "  BehaviorTree tests PASSED" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  PowerGame Unit Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    int passed = 0;
    int failed = 0;

    auto runTest = [&](const std::string& name, void (*test)()) {
        try {
            test();
            passed++;
        } catch (const std::exception& e) {
            std::cerr << "  FAILED: " << name << " - " << e.what() << std::endl;
            failed++;
        } catch (...) {
            std::cerr << "  FAILED: " << name << " - Unknown error" << std::endl;
            failed++;
        }
    };

    runTest("ResourceManager", TestResourceManager);
    runTest("FactionManager", TestFactionManager);
    runTest("IntelligenceManager", TestIntelligenceManager);
    runTest("AllianceManager", TestAllianceManager);
    runTest("CharacterManager", TestCharacterManager);
    runTest("StoryManager", TestStoryManager);
    runTest("StrategyManager", TestStrategyManager);
    runTest("EndingManager", TestEndingManager);
    runTest("AIDecisionEngine", TestAIDecisionEngine);
    runTest("EventBus", TestEventBus);
    runTest("BehaviorTree", TestBehaviorTree);

    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  Results: " << passed << " passed, " << failed << " failed" << std::endl;
    std::cout << "========================================" << std::endl;

    return failed > 0 ? 1 : 0;
}
