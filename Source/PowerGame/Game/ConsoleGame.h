#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <iomanip>
#include "../Core/GameManager.h"
#include "../Core/GameSystem.h"
#include "../Core/EventBus.h"
#include "../Resources/ResourceManager.h"
#include "../Faction/FactionManager.h"
#include "../Intelligence/IntelligenceManager.h"
#include "../Alliance/AllianceManager.h"
#include "../Character/CharacterManager.h"
#include "../Story/StoryManager.h"
#include "../Strategy/StrategyManager.h"
#include "../Ending/EndingManager.h"
#include "../Data/SaveManager.h"
#include "../AI/AIDecisionEngine.h"

namespace PowerGame {

class ConsoleGame {
public:
    static ConsoleGame& Instance() {
        static ConsoleGame instance;
        return instance;
    }

    int Run() {
        Initialize();
        ShowTitleScreen();
        GameLoop();
        return 0;
    }

private:
    ConsoleGame() = default;
    bool running_ = false;
    bool gameActive_ = false;

    void Initialize() {
        auto& gameMgr = GameManager::Instance();
        gameMgr.Initialize();
        running_ = true;
    }

    void ShowTitleScreen() {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║         ⚔  POWER GAME: The Art of Political Intrigue  ⚔     ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║              A Deep Strategy Game of Power & Betrayal        ║\n";
        std::cout << "║                                                              ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        std::cout << "\n";
        std::cout << "  In a nation on the brink of transformation, you must navigate\n";
        std::cout << "  the treacherous waters of political intrigue. Every decision\n";
        std::cout << "  shapes the fate of millions. Will you build a democracy,\n";
        std::cout << "  seize absolute power, or pull strings from the shadows?\n";
        std::cout << "\n";
        std::cout << "  Inspired by: Watergate, Palace Coups, Financial Crises,\n";
        std::cout << "  Cold War Espionage, and the eternal struggle for power.\n";
        std::cout << "\n";
    }

    void GameLoop() {
        while (running_) {
            if (!gameActive_) {
                ShowMainMenu();
            } else {
                ShowGameMenu();
            }
        }
    }

    void ShowMainMenu() {
        std::cout << "┌─────────────────────┐\n";
        std::cout << "│     MAIN MENU       │\n";
        std::cout << "├─────────────────────┤\n";
        std::cout << "│  1. New Game        │\n";
        std::cout << "│  2. Load Game       │\n";
        std::cout << "│  3. How to Play     │\n";
        std::cout << "│  4. Quit            │\n";
        std::cout << "└─────────────────────┘\n";
        std::cout << "> ";

        int choice = ReadInt(1, 4);
        switch (choice) {
            case 1: StartNewGame(); break;
            case 2: LoadGame(); break;
            case 3: ShowTutorial(); break;
            case 4: running_ = false; break;
        }
    }

    void StartNewGame() {
        auto& gameMgr = GameManager::Instance();
        gameMgr.StartNewGame();
        gameActive_ = true;

        std::cout << "\n══════════════════════════════════════════════════════════════\n";
        std::cout << "  A NEW ERA BEGINS\n";
        std::cout << "══════════════════════════════════════════════════════════════\n\n";
        std::cout << "  The Prime Minister's sudden resignation has created a power\n";
        std::cout << "  vacuum. Factions scramble for position. The nation holds its\n";
        std::cout << "  breath, waiting to see who will seize the reins of power.\n\n";
        std::cout << "  Your journey begins now.\n\n";

        ProcessStoryEvents();
    }

    void LoadGame() {
        auto& saveMgr = SaveManager::Instance();
        auto saves = saveMgr.GetSaveList();
        if (saves.empty()) {
            std::cout << "\n  No saved games found.\n\n";
            return;
        }
        std::cout << "\n  Saved Games:\n";
        for (size_t i = 0; i < saves.size(); i++) {
            std::cout << "  " << (i + 1) << ". " << saves[i].saveName
                      << " (Turn " << saves[i].turn << ", " << saves[i].timestamp << ")\n";
        }
        std::cout << "\n  Select save (0 to cancel): ";
        int choice = ReadInt(0, static_cast<int>(saves.size()));
        if (choice > 0) {
            saveMgr.LoadGame(saves[choice - 1].saveId);
            gameActive_ = true;
            std::cout << "\n  Game loaded successfully!\n\n";
        }
    }

    void ShowGameMenu() {
        auto& gameMgr = GameManager::Instance();
        if (gameMgr.GetCurrentState() == GameState::Ending ||
            gameMgr.GetCurrentState() == GameState::GameOver) {
            ShowEnding();
            return;
        }

        auto& gameTime = gameMgr.GetGameTime();
        std::string seasonStr = SeasonToString(gameTime.GetSeason());
        std::string phaseStr = PhaseToString(gameTime.GetPhase());

        std::cout << "\n┌──────────────────────────────────────────────────────────────┐\n";
        std::cout << "│  Turn " << gameMgr.GetCurrentTurn() << "/" << gameMgr.GetMaxTurns()
                  << "  │  Year " << gameTime.GetYear()
                  << "  │  " << seasonStr << "  │  " << phaseStr << "\n";
        std::cout << "├──────────────────────────────────────────────────────────────┤\n";

        auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
        if (resourceMgr) {
            std::cout << "│  💰 Political Capital: " << FormatBar(resourceMgr->GetResource(ResourceType::PoliticalCapital))
                      << "  📊 Influence: " << FormatBar(resourceMgr->GetResource(ResourceType::Influence)) << "\n";
            std::cout << "│  📋 Information: " << FormatBar(resourceMgr->GetResource(ResourceType::Information))
                      << "  💵 Wealth: " << FormatBar(resourceMgr->GetResource(ResourceType::Wealth)) << "\n";
            std::cout << "│  ⚔  Military: " << FormatBar(resourceMgr->GetResource(ResourceType::Military))
                      << "  ⭐ Reputation: " << FormatBar(resourceMgr->GetResource(ResourceType::Reputation)) << "\n";
            std::cout << "│  🏛  Legitimacy: " << FormatBar(resourceMgr->GetResource(ResourceType::Legitimacy))
                      << "  👥 Public Support: " << FormatBar(resourceMgr->GetResource(ResourceType::PublicSupport)) << "\n";
        }

        std::cout << "├──────────────────────────────────────────────────────────────┤\n";
        std::cout << "│  ACTIONS:                                                    │\n";
        std::cout << "│  1. Next Turn          2. View Factions                      │\n";
        std::cout << "│  3. View Characters    4. Intelligence Operations            │\n";
        std::cout << "│  5. Alliance Actions   6. Resource Management               │\n";
        std::cout << "│  7. View Story Log     8. Save Game                          │\n";
        std::cout << "│  9. Strategy Analysis  10. Help                              │\n";
        std::cout << "│  0. Quit to Main Menu                                        │\n";
        std::cout << "└──────────────────────────────────────────────────────────────┘\n";
        std::cout << "> ";

        int choice = ReadInt(0, 10);
        switch (choice) {
            case 0: gameActive_ = false; break;
            case 1: AdvanceTurn(); break;
            case 2: ViewFactions(); break;
            case 3: ViewCharacters(); break;
            case 4: IntelligenceOps(); break;
            case 5: AllianceActions(); break;
            case 6: ResourceManagement(); break;
            case 7: ViewStoryLog(); break;
            case 8: SaveGame(); break;
            case 9: StrategyAnalysis(); break;
            case 10: ShowTutorial(); break;
        }
    }

    void AdvanceTurn() {
        auto& gameMgr = GameManager::Instance();
        gameMgr.AdvanceTurn();
        std::cout << "\n  ═══ Turn " << gameMgr.GetCurrentTurn() << " ═══\n";

        ProcessNPCActions();
        ProcessStoryEvents();

        if (gameMgr.GetCurrentState() == GameState::Ending) {
            ShowEnding();
        }
    }

    void ProcessStoryEvents() {
        auto* storyMgr = GameSystemManager::Instance().GetSystem<StoryManager>();
        if (!storyMgr) return;

        auto available = storyMgr->GetAvailableNodes();
        for (auto* node : available) {
            if (node->completed) continue;

            std::cout << "\n  ╔══════════════════════════════════════════════════════════╗\n";
            std::cout << "  ║  📜 STORY EVENT: " << node->title << "\n";
            std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";

            std::istringstream desc(node->description);
            std::string line;
            while (std::getline(desc, line)) {
                std::cout << "  ║  " << line << "\n";
            }

            std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";
            std::cout << "  ║  CHOICES:\n";

            int idx = 1;
            for (const auto& choice : node->choices) {
                std::cout << "  ║  [" << idx << "] " << choice.text << "\n";
                std::cout << "  ║      " << choice.description << "\n";

                if (!choice.resourceEffects.empty()) {
                    std::cout << "  ║      Effects: ";
                    bool first = true;
                    for (const auto& [res, val] : choice.resourceEffects) {
                        if (!first) std::cout << ", ";
                        std::cout << res << (val >= 0 ? "+" : "") << static_cast<int>(val);
                        first = false;
                    }
                    std::cout << "\n";
                }
                idx++;
            }

            std::cout << "  ╚══════════════════════════════════════════════════════════╝\n";
            std::cout << "  Choose (1-" << node->choices.size() << "): ";

            int choiceIdx = ReadInt(1, static_cast<int>(node->choices.size()));
            const auto& selectedChoice = node->choices[choiceIdx - 1];

            storyMgr->MakeChoice(node->id, selectedChoice.id);

            std::cout << "\n  ✦ You chose: " << selectedChoice.text << "\n";
            std::cout << "  \"" << selectedChoice.description << "\"\n\n";

            PauseForInput();
        }
    }

    void ProcessNPCActions() {
        auto* charMgr = GameSystemManager::Instance().GetSystem<CharacterManager>();
        if (!charMgr) return;

        auto characters = charMgr->GetActiveCharacters();
        int actionsShown = 0;

        for (auto* character : characters) {
            if (actionsShown >= 5) break;
            auto decision = charMgr->GetCharacterDecision(character->GetId(),
                GameManager::Instance().GetCurrentTurn());

            if (decision.type == CharacterAI::DecisionType::Betrayal ||
                decision.type == CharacterAI::DecisionType::Manipulation ||
                decision.type == CharacterAI::DecisionType::Oppose) {
                std::cout << "  🗣 " << character->GetName() << " is "
                          << DecisionTypeToString(decision.type) << "\n";
                actionsShown++;
            }
        }
        if (actionsShown > 0) std::cout << "\n";
    }

    void ViewFactions() {
        auto* factionMgr = GameSystemManager::Instance().GetSystem<FactionManager>();
        if (!factionMgr) return;

        std::cout << "\n  ╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "  ║  FACTION OVERVIEW                                        ║\n";
        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";

        for (const auto& [id, faction] : factionMgr->GetAllFactions()) {
            if (!faction->IsActive()) continue;
            std::cout << "  ║  " << faction->GetName() << " [" << IdeologyToString(faction->GetIdeology()) << "]\n";
            std::cout << "  ║    Power: " << FormatBar(faction->GetPower())
                      << "  Cohesion: " << FormatBar(faction->GetCohesion())
                      << "  Influence: " << FormatBar(faction->GetInfluence()) << "\n";
            std::cout << "  ║    Members: " << faction->GetMemberCount() << "\n";
        }

        float balance = factionMgr->GetFactionBalance();
        std::string dominant = factionMgr->GetDominantFaction();
        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "  ║  Balance Index: " << std::fixed << std::setprecision(1) << (balance * 100) << "%";
        std::cout << "  Dominant: " << dominant << "\n";
        std::cout << "  ╚══════════════════════════════════════════════════════════╝\n\n";

        std::cout << "  [1] Recruit Talent  [2] Manage Loyalty  [0] Back\n> ";
        int choice = ReadInt(0, 2);
        if (choice == 1) {
            RecruitTalent();
        } else if (choice == 2) {
            ManageLoyalty();
        }
    }

    void RecruitTalent() {
        auto* factionMgr = GameSystemManager::Instance().GetSystem<FactionManager>();
        if (!factionMgr) return;

        auto talents = factionMgr->GetTalentRecruitment().GetAvailableTalents();
        if (talents.empty()) {
            std::cout << "\n  No talents available for recruitment at this time.\n\n";
            return;
        }

        std::cout << "\n  Available Talents:\n";
        for (size_t i = 0; i < std::min(talents.size(), size_t(5)); i++) {
            const auto& t = talents[i];
            std::cout << "  [" << (i + 1) << "] " << t.name
                      << " | Skill: " << static_cast<int>(t.skill)
                      << " | Potential: " << static_cast<int>(t.potential)
                      << " | Cost: " << static_cast<int>(t.cost)
                      << " | Prefers: " << IdeologyToString(t.preferredIdeology) << "\n";
        }

        std::cout << "\n  Select talent to recruit (0 to cancel): ";
        int choice = ReadInt(0, static_cast<int>(std::min(talents.size(), size_t(5))));
        if (choice > 0) {
            TalentRecruitment::RecruitmentOffer offer;
            offer.targetId = talents[choice - 1].characterId;
            offer.factionId = "player";
            offer.offeredWealth = 20.0f;
            offer.offeredInfluence = 10.0f;
            offer.offeredPosition = 15.0f;
            offer.successChance = factionMgr->GetTalentRecruitment().CalculateSuccessChance(offer);

            std::cout << "  Success chance: " << static_cast<int>(offer.successChance * 100) << "%\n";
            std::cout << "  Attempt recruitment? (1=Yes, 0=No): ";
            if (ReadInt(0, 1) == 1) {
                bool success = factionMgr->GetTalentRecruitment().ExecuteRecruitment(offer);
                if (success) {
                    std::cout << "\n  ✦ Recruitment successful! " << talents[choice - 1].name
                              << " has joined your cause.\n\n";
                } else {
                    std::cout << "\n  ✗ Recruitment failed. The talent declined your offer.\n\n";
                }
            }
        }
    }

    void ManageLoyalty() {
        auto* factionMgr = GameSystemManager::Instance().GetSystem<FactionManager>();
        auto* charMgr = GameSystemManager::Instance().GetSystem<CharacterManager>();
        if (!factionMgr || !charMgr) return;

        auto characters = charMgr->GetActiveCharacters();
        std::cout << "\n  Character Loyalty Status:\n";
        for (size_t i = 0; i < std::min(characters.size(), size_t(10)); i++) {
            auto* c = characters[i];
            float loyalty = factionMgr->GetLoyaltySystem().GetLoyalty(c->GetId(), c->GetFactionId());
            float defectionRisk = factionMgr->GetLoyaltySystem().GetDefectionRisk(c->GetId(), c->GetFactionId());
            std::cout << "  " << c->GetName() << " [" << c->GetFactionId() << "]"
                      << " Loyalty: " << static_cast<int>(loyalty)
                      << "% | Defection Risk: " << static_cast<int>(defectionRisk) << "%\n";
        }
        std::cout << "\n";
        PauseForInput();
    }

    void ViewCharacters() {
        auto* charMgr = GameSystemManager::Instance().GetSystem<CharacterManager>();
        if (!charMgr) return;

        auto characters = charMgr->GetActiveCharacters();

        std::cout << "\n  ╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "  ║  CHARACTERS (" << characters.size() << " active)                                  ║\n";
        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";

        for (auto* c : characters) {
            const auto& attrs = c->GetAttributes();
            std::cout << "  ║  " << c->GetName() << " [" << StanceToString(c->GetStance()) << "]"
                      << " - " << c->GetFactionId() << "\n";
            std::cout << "  ║    Power Score: " << static_cast<int>(attrs.GetPowerScore())
                      << " | Trust Score: " << static_cast<int>(attrs.GetTrustScore()) << "\n";
            std::cout << "  ║    SCH:" << static_cast<int>(attrs.scheming)
                      << " CHR:" << static_cast<int>(attrs.charisma)
                      << " INT:" << static_cast<int>(attrs.intelligence)
                      << " WLT:" << static_cast<int>(attrs.wealth)
                      << " MIL:" << static_cast<int>(attrs.military) << "\n";
            std::cout << "  ║    REP:" << static_cast<int>(attrs.reputation)
                      << " HON:" << static_cast<int>(attrs.integrity)
                      << " AMB:" << static_cast<int>(attrs.ambition) << "\n";

            if (c->GetHiddenMotive().revealed) {
                std::cout << "  ║    ⚠ Hidden Agenda: " << c->GetHiddenMotive().hiddenAgenda << "\n";
                std::cout << "  ║    Betrayal Tendency: " << static_cast<int>(c->GetHiddenMotive().betrayalTendency) << "%\n";
            }
        }

        std::cout << "  ╚══════════════════════════════════════════════════════════╝\n\n";

        std::cout << "  [1] Investigate Character  [2] View Relationships  [0] Back\n> ";
        int choice = ReadInt(0, 2);
        if (choice == 1) {
            InvestigateCharacter();
        } else if (choice == 2) {
            ViewRelationships();
        }
    }

    void InvestigateCharacter() {
        auto* charMgr = GameSystemManager::Instance().GetSystem<CharacterManager>();
        auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
        if (!charMgr || !resourceMgr) return;

        auto characters = charMgr->GetActiveCharacters();
        std::cout << "\n  Select character to investigate:\n";
        for (size_t i = 0; i < std::min(characters.size(), size_t(10)); i++) {
            std::cout << "  [" << (i + 1) << "] " << characters[i]->GetName() << "\n";
        }
        std::cout << "  [0] Cancel\n> ";

        int choice = ReadInt(0, static_cast<int>(std::min(characters.size(), size_t(10))));
        if (choice > 0) {
            auto* c = characters[choice - 1];
            if (resourceMgr->GetResource(ResourceType::Information) >= 10.0f) {
                resourceMgr->ModifyResource(ResourceType::Information, -10.0f, "Investigation");
                c->RevealMotive();

                std::cout << "\n  ═══ Investigation Report ═══\n";
                std::cout << "  Subject: " << c->GetName() << "\n";
                std::cout << "  Hidden Agenda: " << c->GetHiddenMotive().hiddenAgenda << "\n";
                std::cout << "  Secret Relationship: " << c->GetHiddenMotive().secretRelationship << "\n";
                std::cout << "  Betrayal Tendency: " << static_cast<int>(c->GetHiddenMotive().betrayalTendency) << "%\n";
                std::cout << "  Trigger Condition: " << c->GetHiddenMotive().triggerCondition << "\n\n";
            } else {
                std::cout << "\n  ✗ Insufficient Information resources (need 10).\n\n";
            }
        }
    }

    void ViewRelationships() {
        auto* charMgr = GameSystemManager::Instance().GetSystem<CharacterManager>();
        if (!charMgr) return;

        auto characters = charMgr->GetActiveCharacters();
        std::cout << "\n  Select character:\n";
        for (size_t i = 0; i < std::min(characters.size(), size_t(10)); i++) {
            std::cout << "  [" << (i + 1) << "] " << characters[i]->GetName() << "\n";
        }
        std::cout << "  [0] Cancel\n> ";

        int choice = ReadInt(0, static_cast<int>(std::min(characters.size(), size_t(10))));
        if (choice > 0) {
            auto* c = characters[choice - 1];
            std::cout << "\n  " << c->GetName() << "'s Relationships:\n";
            for (auto* other : characters) {
                if (other->GetId() == c->GetId()) continue;
                float fav = c->GetFavorability(other->GetId());
                float trust = c->GetTrust(other->GetId());
                float threat = c->GetThreatPerception(other->GetId());
                if (fav != 50.0f || trust != 50.0f || threat != 30.0f) {
                    std::cout << "    → " << other->GetName()
                              << " | Favor: " << static_cast<int>(fav)
                              << " | Trust: " << static_cast<int>(trust)
                              << " | Threat: " << static_cast<int>(threat) << "\n";
                }
            }
            std::cout << "\n";
        }
        PauseForInput();
    }

    void IntelligenceOps() {
        std::cout << "\n  ╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "  ║  INTELLIGENCE OPERATIONS                                  ║\n";
        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "  ║  1. Deploy Spy                                           ║\n";
        std::cout << "  ║  2. Recall Spy                                           ║\n";
        std::cout << "  ║  3. Gather Intelligence                                  ║\n";
        std::cout << "  ║  4. Analyze Intel                                        ║\n";
        std::cout << "  ║  5. Security Sweep                                       ║\n";
        std::cout << "  ║  6. View Spy Network                                     ║\n";
        std::cout << "  ║  0. Back                                                 ║\n";
        std::cout << "  ╚══════════════════════════════════════════════════════════╝\n> ";

        int choice = ReadInt(0, 6);
        switch (choice) {
            case 1: DeploySpy(); break;
            case 2: RecallSpy(); break;
            case 3: GatherIntel(); break;
            case 4: AnalyzeIntel(); break;
            case 5: SecuritySweep(); break;
            case 6: ViewSpyNetwork(); break;
        }
    }

    void DeploySpy() {
        auto* intelMgr = GameSystemManager::Instance().GetSystem<IntelligenceManager>();
        auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
        if (!intelMgr || !resourceMgr) return;

        if (resourceMgr->GetResource(ResourceType::Wealth) < 15.0f) {
            std::cout << "\n  ✗ Insufficient Wealth to deploy spy (need 15).\n\n";
            return;
        }

        std::vector<std::string> factions = {"conservatives", "liberals", "neutrals", "radicals", "opportunists"};
        std::cout << "\n  Select target faction:\n";
        for (size_t i = 0; i < factions.size(); i++) {
            std::cout << "  [" << (i + 1) << "] " << factions[i] << "\n";
        }
        std::cout << "> ";
        int choice = ReadInt(1, static_cast<int>(factions.size()));

        resourceMgr->ModifyResource(ResourceType::Wealth, -15.0f, "Deploy spy");
        auto* spy = intelMgr->DeploySpy("Agent_" + std::to_string(GameManager::Instance().GetCurrentTurn()),
            factions[choice - 1], "player", 50.0f + (resourceMgr->GetResource(ResourceType::Information) * 0.3f));

        std::cout << "\n  ✦ Spy deployed to " << factions[choice - 1]
                  << " | Skill: " << static_cast<int>(spy->skill)
                  << " | Exposure Risk: " << static_cast<int>(spy->exposureRisk) << "%\n\n";
    }

    void RecallSpy() {
        auto* intelMgr = GameSystemManager::Instance().GetSystem<IntelligenceManager>();
        if (!intelMgr) return;

        const auto& spies = intelMgr->GetAllSpies();
        std::vector<const SpyAgent*> activeSpies;
        for (const auto& [id, spy] : spies) {
            if (spy.active) activeSpies.push_back(&spy);
        }

        if (activeSpies.empty()) {
            std::cout << "\n  No active spies to recall.\n\n";
            return;
        }

        std::cout << "\n  Active Spies:\n";
        for (size_t i = 0; i < activeSpies.size(); i++) {
            const auto* s = activeSpies[i];
            std::cout << "  [" << (i + 1) << "] " << s->name
                      << " → " << s->targetFactionId
                      << " | Skill: " << static_cast<int>(s->skill)
                      << " | Risk: " << static_cast<int>(s->exposureRisk) << "%\n";
        }
        std::cout << "  [0] Cancel\n> ";

        int choice = ReadInt(0, static_cast<int>(activeSpies.size()));
        if (choice > 0) {
            intelMgr->RecallSpy(activeSpies[choice - 1]->id);
            std::cout << "\n  ✦ Spy recalled safely.\n\n";
        }
    }

    void GatherIntel() {
        auto* intelMgr = GameSystemManager::Instance().GetSystem<IntelligenceManager>();
        if (!intelMgr) return;

        const auto& spies = intelMgr->GetAllSpies();
        std::vector<const SpyAgent*> activeSpies;
        for (const auto& [id, spy] : spies) {
            if (spy.active) activeSpies.push_back(&spy);
        }

        if (activeSpies.empty()) {
            std::cout << "\n  No active spies available. Deploy a spy first.\n\n";
            return;
        }

        std::cout << "\n  Select spy to gather intel:\n";
        for (size_t i = 0; i < activeSpies.size(); i++) {
            std::cout << "  [" << (i + 1) << "] " << activeSpies[i]->name
                      << " → " << activeSpies[i]->targetFactionId << "\n";
        }
        std::cout << "> ";
        int choice = ReadInt(1, static_cast<int>(activeSpies.size()));

        std::vector<std::string> intelTypes = {"PoliticalSecret", "FinancialRecord", "MilitaryIntel",
                                                 "PersonalScandal", "AllianceDetail"};
        std::cout << "  Intel type:\n";
        for (size_t i = 0; i < intelTypes.size(); i++) {
            std::cout << "  [" << (i + 1) << "] " << intelTypes[i] << "\n";
        }
        std::cout << "> ";
        int typeChoice = ReadInt(1, static_cast<int>(intelTypes.size()));

        auto intel = intelMgr->GatherIntel(activeSpies[choice - 1]->id,
            static_cast<IntelType>(typeChoice - 1), activeSpies[choice - 1]->targetFactionId);

        std::cout << "\n  ═══ Intelligence Report ═══\n";
        std::cout << "  Type: " << intelTypes[typeChoice - 1] << "\n";
        std::cout << "  Reliability: " << static_cast<int>(intel.reliability) << "%\n";
        std::cout << "  Value: " << static_cast<int>(intel.value) << "\n";
        std::cout << "  Content: " << intel.content << "\n";
        if (intel.isDisinformation) {
            std::cout << "  ⚠ WARNING: This intel may be disinformation!\n";
        }
        std::cout << "\n";
    }

    void AnalyzeIntel() {
        auto* intelMgr = GameSystemManager::Instance().GetSystem<IntelligenceManager>();
        auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
        if (!intelMgr || !resourceMgr) return;

        if (resourceMgr->GetResource(ResourceType::Information) < 5.0f) {
            std::cout << "\n  ✗ Insufficient Information to analyze (need 5).\n\n";
            return;
        }

        resourceMgr->ModifyResource(ResourceType::Information, -5.0f, "Analysis");
        std::cout << "\n  Intel analysis complete. Your understanding of the political landscape has improved.\n\n";
    }

    void SecuritySweep() {
        auto* intelMgr = GameSystemManager::Instance().GetSystem<IntelligenceManager>();
        auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
        if (!intelMgr || !resourceMgr) return;

        if (resourceMgr->GetResource(ResourceType::Wealth) < 10.0f) {
            std::cout << "\n  ✗ Insufficient Wealth for security sweep (need 10).\n\n";
            return;
        }

        resourceMgr->ModifyResource(ResourceType::Wealth, -10.0f, "Security sweep");
        auto result = intelMgr->PerformSweep("player", 60.0f);

        std::cout << "\n  ✦ Security sweep complete.\n";
        std::cout << "  Discovered spies: " << result.discoveredSpies.size() << "\n";
        std::cout << "  Security improvement: +" << static_cast<int>(result.securityImprovement) << "\n\n";
    }

    void ViewSpyNetwork() {
        auto* intelMgr = GameSystemManager::Instance().GetSystem<IntelligenceManager>();
        if (!intelMgr) return;

        std::cout << "\n  ═══ Spy Network Status ═══\n";
        const auto& spies = intelMgr->GetAllSpies();
        if (spies.empty()) {
            std::cout << "  No spies deployed.\n";
        }
        for (const auto& [id, spy] : spies) {
            std::cout << "  " << spy.name << (spy.active ? " [ACTIVE]" : " [INACTIVE]")
                      << " → " << spy.targetFactionId
                      << " | Skill: " << static_cast<int>(spy.skill)
                      << " | Risk: " << static_cast<int>(spy.exposureRisk) << "%"
                      << " | Turns: " << spy.turnsEmbedded << "\n";
        }

        const auto& networks = intelMgr->GetAllNetworks();
        for (const auto& [id, net] : networks) {
            std::cout << "  Network: " << net.name
                      << " | Coverage: " << static_cast<int>(net.coverage) << "%"
                      << " | Efficiency: " << static_cast<int>(net.efficiency) << "%\n";
        }
        std::cout << "\n";
        PauseForInput();
    }

    void AllianceActions() {
        std::cout << "\n  ╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "  ║  ALLIANCE & DIPLOMACY                                    ║\n";
        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "  ║  1. Propose Treaty                                       ║\n";
        std::cout << "  ║  2. View Active Treaties                                 ║\n";
        std::cout << "  ║  3. Evaluate Trust                                       ║\n";
        std::cout << "  ║  4. Execute Betrayal                                     ║\n";
        std::cout << "  ║  0. Back                                                 ║\n";
        std::cout << "  ╚══════════════════════════════════════════════════════════╝\n> ";

        int choice = ReadInt(0, 4);
        switch (choice) {
            case 1: ProposeTreaty(); break;
            case 2: ViewTreaties(); break;
            case 3: EvaluateTrust(); break;
            case 4: ExecuteBetrayal(); break;
        }
    }

    void ProposeTreaty() {
        auto* allianceMgr = GameSystemManager::Instance().GetSystem<AllianceManager>();
        if (!allianceMgr) return;

        std::vector<std::string> factions = {"conservatives", "liberals", "neutrals", "radicals", "opportunists"};
        std::cout << "\n  Select ally faction:\n";
        for (size_t i = 0; i < factions.size(); i++) {
            std::cout << "  [" << (i + 1) << "] " << factions[i] << "\n";
        }
        std::cout << "> ";
        int factionChoice = ReadInt(1, static_cast<int>(factions.size()));

        std::vector<std::string> treatyTypes = {"MilitaryPact", "EconomicAlliance", "PoliticalCoalition",
                                                  "IntelligenceSharing", "NonAggressionPact", "TradeAgreement"};
        std::cout << "  Treaty type:\n";
        for (size_t i = 0; i < treatyTypes.size(); i++) {
            std::cout << "  [" << (i + 1) << "] " << treatyTypes[i] << "\n";
        }
        std::cout << "> ";
        int typeChoice = ReadInt(1, static_cast<int>(treatyTypes.size()));

        auto* treaty = allianceMgr->ProposeTreaty(
            static_cast<AllianceType>(typeChoice - 1),
            "player", factions[factionChoice - 1], {}, 20);

        if (treaty) {
            float trust = allianceMgr->GetFactionTrustLevel("player", factions[factionChoice - 1]);
            bool accepted = trust > 40.0f;

            if (accepted) {
                allianceMgr->AcceptTreaty(treaty->GetId());
                std::cout << "\n  ✦ Treaty accepted! " << treatyTypes[typeChoice - 1]
                          << " with " << factions[factionChoice - 1] << " is now active.\n\n";
            } else {
                std::cout << "\n  ✗ Treaty rejected. Trust level too low ("
                          << static_cast<int>(trust) << "%).\n\n";
            }
        }
    }

    void ViewTreaties() {
        auto* allianceMgr = GameSystemManager::Instance().GetSystem<AllianceManager>();
        if (!allianceMgr) return;

        std::cout << "\n  ═══ Active Treaties ═══\n";
        auto treaties = allianceMgr->GetActiveTreatiesForFaction("player");
        if (treaties.empty()) {
            std::cout << "  No active treaties.\n";
        }
        for (auto* treaty : treaties) {
            std::cout << "  " << treaty->GetId() << " | Type: " << static_cast<int>(treaty->GetType())
                      << " | With: " << treaty->GetFactionA() << "-" << treaty->GetFactionB()
                      << " | Trust: " << static_cast<int>(treaty->GetTrustLevel()) << "%"
                      << " | Turns left: " << treaty->GetRemainingTurns() << "\n";
        }
        std::cout << "\n";
        PauseForInput();
    }

    void EvaluateTrust() {
        auto* allianceMgr = GameSystemManager::Instance().GetSystem<AllianceManager>();
        if (!allianceMgr) return;

        std::vector<std::string> factions = {"conservatives", "liberals", "neutrals", "radicals", "opportunists"};
        std::cout << "\n  Trust Evaluation:\n";
        for (const auto& faction : factions) {
            auto eval = allianceMgr->EvaluateTrust("player", faction);
            std::cout << "  " << faction
                      << " | Trust: " << static_cast<int>(eval.trustScore) << "%"
                      << " | Reliability: " << static_cast<int>(eval.reliabilityScore) << "%"
                      << " | Risk: " << static_cast<int>(eval.riskAssessment) << "%\n";
        }
        std::cout << "\n";
        PauseForInput();
    }

    void ExecuteBetrayal() {
        auto* allianceMgr = GameSystemManager::Instance().GetSystem<AllianceManager>();
        if (!allianceMgr) return;

        auto treaties = allianceMgr->GetActiveTreatiesForFaction("player");
        if (treaties.empty()) {
            std::cout << "\n  No active treaties to betray.\n\n";
            return;
        }

        std::cout << "\n  ⚠ WARNING: Betrayal has severe consequences!\n";
        std::cout << "  Select treaty to betray:\n";
        for (size_t i = 0; i < treaties.size(); i++) {
            std::cout << "  [" << (i + 1) << "] " << treaties[i]->GetId() << "\n";
        }
        std::cout << "  [0] Cancel\n> ";

        int choice = ReadInt(0, static_cast<int>(treaties.size()));
        if (choice > 0) {
            std::cout << "  Confirm betrayal? This cannot be undone! (1=Yes, 0=No): ";
            if (ReadInt(0, 1) == 1) {
                std::string victimId = treaties[choice - 1]->GetFactionA() == "player" ?
                    treaties[choice - 1]->GetFactionB() : treaties[choice - 1]->GetFactionA();
                allianceMgr->ExecuteBetrayal("player", victimId, treaties[choice - 1]->GetId(), 60.0f, "Strategic betrayal");
                std::cout << "\n  ⚡ Betrayal executed! Your reputation will suffer.\n\n";
            }
        }
    }

    void ResourceManagement() {
        auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
        if (!resourceMgr) return;

        std::cout << "\n  ╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "  ║  RESOURCE MANAGEMENT                                     ║\n";
        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";

        auto resources = resourceMgr->GetAllResources();
        auto regenRates = resourceMgr->GetRegenRates();

        std::vector<std::string> resNames = {"PoliticalCapital", "Influence", "Information",
                                               "Wealth", "Military", "Reputation", "Legitimacy", "PublicSupport"};
        for (size_t i = 0; i < resNames.size(); i++) {
            auto type = static_cast<ResourceType>(i);
            std::cout << "  ║  " << resNames[i] << ": " << static_cast<int>(resourceMgr->GetResource(type))
                      << "/100 (+" << std::fixed << std::setprecision(1) << resourceMgr->GetRegenRates().at(type) << "/turn)\n";
        }

        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "  ║  1. Convert Resources                                    ║\n";
        std::cout << "  ║  2. Invest Wealth (gain Political Capital)               ║\n";
        std::cout << "  ║  3. Fund Intelligence (gain Information)                 ║\n";
        std::cout << "  ║  4. Public Campaign (gain Public Support)                ║\n";
        std::cout << "  ║  0. Back                                                 ║\n";
        std::cout << "  ╚══════════════════════════════════════════════════════════╝\n> ";

        int choice = ReadInt(0, 4);
        switch (choice) {
            case 1: ConvertResources(); break;
            case 2:
                if (resourceMgr->GetResource(ResourceType::Wealth) >= 20.0f) {
                    resourceMgr->ConvertResource(ResourceType::Wealth, ResourceType::PoliticalCapital, 10.0f);
                    std::cout << "\n  ✦ Converted Wealth to Political Capital.\n\n";
                } else {
                    std::cout << "\n  ✗ Insufficient Wealth.\n\n";
                }
                break;
            case 3:
                if (resourceMgr->GetResource(ResourceType::Wealth) >= 25.0f) {
                    resourceMgr->ConvertResource(ResourceType::Wealth, ResourceType::Information, 15.0f);
                    std::cout << "\n  ✦ Funded intelligence operations.\n\n";
                } else {
                    std::cout << "\n  ✗ Insufficient Wealth.\n\n";
                }
                break;
            case 4:
                if (resourceMgr->GetResource(ResourceType::Influence) >= 15.0f) {
                    resourceMgr->ConvertResource(ResourceType::Influence, ResourceType::PublicSupport, 10.0f);
                    std::cout << "\n  ✦ Launched public campaign.\n\n";
                } else {
                    std::cout << "\n  ✗ Insufficient Influence.\n\n";
                }
                break;
        }
    }

    void ConvertResources() {
        auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
        if (!resourceMgr) return;

        std::cout << "\n  Convert from:\n";
        std::cout << "  [1] Wealth  [2] Information  [3] Political Capital  [4] Influence  [0] Cancel\n> ";
        int fromChoice = ReadInt(0, 4);
        if (fromChoice == 0) return;

        std::cout << "  Convert to:\n";
        std::cout << "  [1] Political Capital  [2] Influence  [3] Information  [4] Military  [0] Cancel\n> ";
        int toChoice = ReadInt(0, 4);
        if (toChoice == 0) return;

        ResourceType fromType = static_cast<ResourceType>(fromChoice - 1);
        ResourceType toType = static_cast<ResourceType>(toChoice + 3);

        float amount = 10.0f;
        if (resourceMgr->ConvertResource(fromType, toType, amount)) {
            std::cout << "\n  ✦ Resource conversion successful.\n\n";
        } else {
            std::cout << "\n  ✗ Conversion failed. Check resources.\n\n";
        }
    }

    void ViewStoryLog() {
        auto* storyMgr = GameSystemManager::Instance().GetSystem<StoryManager>();
        if (!storyMgr) return;

        const auto& history = storyMgr->GetDecisionHistory();
        std::cout << "\n  ═══ Decision History ═══\n";
        if (history.empty()) {
            std::cout << "  No decisions made yet.\n";
        }
        for (const auto& decision : history) {
            std::cout << "  • " << decision << "\n";
        }
        std::cout << "\n";
        PauseForInput();
    }

    void StrategyAnalysis() {
        auto* endingMgr = GameSystemManager::Instance().GetSystem<EndingManager>();
        auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
        if (!endingMgr || !resourceMgr) return;

        std::cout << "\n  ╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "  ║  STRATEGY ANALYSIS                                       ║\n";
        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";

        auto evaluations = endingMgr->EvaluateAllEndings();
        std::sort(evaluations.begin(), evaluations.end(),
            [](const Ending& a, const Ending& b) { return a.score > b.score; });

        std::cout << "  ║  Ending Probabilities:\n";
        for (const auto& ending : evaluations) {
            int barLen = static_cast<int>(ending.score / 5);
            std::cout << "  ║  " << ending.title << ": " << std::fixed << std::setprecision(1) << ending.score << "% ";
            std::cout << "[";
            for (int i = 0; i < 20; i++) std::cout << (i < barLen ? "█" : "░");
            std::cout << "]\n";
        }

        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "  ║  Resource Strengths:\n";
        auto resources = resourceMgr->GetAllResources();
        std::string strongest;
        float maxRes = 0;
        std::string weakest;
        float minRes = 101;
        for (const auto& [type, value] : resources) {
            if (value > maxRes) { maxRes = value; strongest = ResourceTypeToString(type); }
            if (value < minRes) { minRes = value; weakest = ResourceTypeToString(type); }
        }
        std::cout << "  ║  Strongest: " << strongest << " (" << static_cast<int>(maxRes) << ")\n";
        std::cout << "  ║  Weakest: " << weakest << " (" << static_cast<int>(minRes) << ")\n";

        std::cout << "  ╚══════════════════════════════════════════════════════════╝\n\n";
        PauseForInput();
    }

    void SaveGame() {
        auto& saveMgr = SaveManager::Instance();
        std::cout << "\n  Enter save name: ";
        std::string name;
        std::getline(std::cin, name);
        if (name.empty()) name = "Save_" + std::to_string(GameManager::Instance().GetCurrentTurn());

        if (saveMgr.SaveGame(name)) {
            std::cout << "  ✦ Game saved successfully!\n\n";
        } else {
            std::cout << "  ✗ Failed to save game.\n\n";
        }
    }

    void ShowEnding() {
        auto* endingMgr = GameSystemManager::Instance().GetSystem<EndingManager>();
        auto* ending = endingMgr ? endingMgr->GetTriggeredEnding() : nullptr;

        std::cout << "\n";
        std::cout << "  ╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "  ║                                                            ║\n";
        std::cout << "  ║              ⚜  GAME OVER  ⚜                              ║\n";
        std::cout << "  ║                                                            ║\n";

        if (ending) {
            std::cout << "  ║  Ending: " << ending->title << "\n";
            std::cout << "  ║                                                            ║\n";

            std::istringstream desc(ending->description);
            std::string line;
            while (std::getline(desc, line)) {
                std::cout << "  ║  " << line << "\n";
            }
            std::cout << "  ║                                                            ║\n";

            std::istringstream epilogue(ending->epilogue);
            while (std::getline(epilogue, line)) {
                std::cout << "  ║  " << line << "\n";
            }
        } else {
            std::cout << "  ║  The political landscape has shifted beyond recovery.     ║\n";
            std::cout << "  ║  Your legacy remains uncertain.                           ║\n";
        }

        std::cout << "  ║                                                            ║\n";
        std::cout << "  ╚══════════════════════════════════════════════════════════╝\n\n";

        if (endingMgr) {
            auto evaluations = endingMgr->EvaluateAllEndings();
            std::cout << "  All Ending Scores:\n";
            for (const auto& e : evaluations) {
                std::cout << "    " << e.title << ": " << std::fixed << std::setprecision(1) << e.score << "%\n";
            }
        }
        std::cout << "\n";

        gameActive_ = false;
        GameManager::Instance().SetState(GameState::MainMenu);
        PauseForInput();
    }

    void ShowTutorial() {
        std::cout << "\n  ╔══════════════════════════════════════════════════════════╗\n";
        std::cout << "  ║  HOW TO PLAY                                             ║\n";
        std::cout << "  ╠══════════════════════════════════════════════════════════╣\n";
        std::cout << "  ║                                                            ║\n";
        std::cout << "  ║  OBJECTIVE: Navigate political intrigue to shape the       ║\n";
        std::cout << "  ║  nation's destiny. Your choices determine one of 8         ║\n";
        std::cout << "  ║  possible endings.                                         ║\n";
        std::cout << "  ║                                                            ║\n";
        std::cout << "  ║  CORE RESOURCES:                                           ║\n";
        std::cout << "  ║  • Political Capital - Primary currency for actions        ║\n";
        std::cout << "  ║  • Influence - Ability to sway events                      ║\n";
        std::cout << "  ║  • Information - Enables informed decisions                ║\n";
        std::cout << "  ║  • Wealth - Economic power, converts to other resources    ║\n";
        std::cout << "  ║  • Military - Enables forceful actions                     ║\n";
        std::cout << "  ║  • Reputation - Public standing                            ║\n";
        std::cout << "  ║  • Legitimacy - Right to rule, critical for stability      ║\n";
        std::cout << "  ║  • Public Support - Popular backing                        ║\n";
        std::cout << "  ║                                                            ║\n";
        std::cout << "  ║  KEY MECHANICS:                                            ║\n";
        std::cout << "  ║  • Story events present critical choices each turn         ║\n";
        std::cout << "  ║  • Deploy spies to gather intelligence on factions         ║\n";
        std::cout << "  ║  • Form alliances to strengthen your position              ║\n";
        std::cout << "  ║  • Investigate characters to reveal hidden motives         ║\n";
        std::cout << "  ║  • Convert resources to focus your strategy                ║\n";
        std::cout << "  ║  • Use Strategy Analysis to track your path                ║\n";
        std::cout << "  ║                                                            ║\n";
        std::cout << "  ║  ENDINGS (8 total):                                        ║\n";
        std::cout << "  ║  Dictatorship, Democracy, Power Balance, Exile,            ║\n";
        std::cout << "  ║  Shadow Control, System Collapse, Foreign Intervention,    ║\n";
        std::cout << "  ║  Personal Ambition                                         ║\n";
        std::cout << "  ║                                                            ║\n";
        std::cout << "  ║  TIP: Balance short-term gains with long-term strategy.    ║\n";
        std::cout << "  ║  Every decision has consequences that may not appear        ║\n";
        std::cout << "  ║  until several turns later!                                ║\n";
        std::cout << "  ║                                                            ║\n";
        std::cout << "  ╚══════════════════════════════════════════════════════════╝\n\n";
        PauseForInput();
    }

    static std::string FormatBar(float value) {
        int bars = static_cast<int>(value / 5);
        std::string result = std::to_string(static_cast<int>(value)) + "% [";
        for (int i = 0; i < 20; i++) result += (i < bars ? "█" : "░");
        result += "]";
        return result;
    }

    static std::string SeasonToString(Season s) {
        switch (s) {
            case Season::Spring: return "Spring";
            case Season::Summer: return "Summer";
            case Season::Autumn: return "Autumn";
            case Season::Winter: return "Winter";
            default: return "?";
        }
    }

    static std::string PhaseToString(Phase p) {
        switch (p) {
            case Phase::Morning: return "Morning";
            case Phase::Afternoon: return "Afternoon";
            case Phase::Evening: return "Evening";
            case Phase::Night: return "Night";
            default: return "?";
        }
    }

    static std::string StanceToString(PoliticalStance s) {
        switch (s) {
            case PoliticalStance::Conservative: return "CON";
            case PoliticalStance::Liberal: return "LIB";
            case PoliticalStance::Neutral: return "NEU";
            case PoliticalStance::Radical: return "RAD";
            case PoliticalStance::Opportunist: return "OPP";
            default: return "?";
        }
    }

    static std::string IdeologyToString(FactionIdeology i) {
        switch (i) {
            case FactionIdeology::Conservative: return "Conservative";
            case FactionIdeology::Liberal: return "Liberal";
            case FactionIdeology::Neutral: return "Neutral";
            case FactionIdeology::Radical: return "Radical";
            case FactionIdeology::Opportunist: return "Opportunist";
            default: return "?";
        }
    }

    static std::string DecisionTypeToString(CharacterAI::DecisionType dt) {
        switch (dt) {
            case CharacterAI::DecisionType::Support: return "supporting the establishment";
            case CharacterAI::DecisionType::Oppose: return "opposing current policies";
            case CharacterAI::DecisionType::Neutrality: return "remaining neutral";
            case CharacterAI::DecisionType::Betrayal: return "planning a betrayal";
            case CharacterAI::DecisionType::Alliance: return "seeking alliances";
            case CharacterAI::DecisionType::Manipulation: return "manipulating behind the scenes";
            case CharacterAI::DecisionType::SelfPreservation: return "protecting their own interests";
            default: return "deliberating";
        }
    }

    static int ReadInt(int minVal, int maxVal) {
        int value;
        while (true) {
            std::string input;
            std::getline(std::cin, input);
            try {
                value = std::stoi(input);
                if (value >= minVal && value <= maxVal) return value;
            } catch (...) {}
            std::cout << "  Invalid input. Enter a number between "
                      << minVal << " and " << maxVal << ": ";
        }
    }

    static void PauseForInput() {
        std::cout << "  Press Enter to continue...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
};

}
