#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <random>
#include "../Core/GameSystem.h"
#include "../Core/EventBus.h"
#include "../Faction/FactionManager.h"

namespace PowerGame {

enum class PoliticalStance {
    Conservative,
    Liberal,
    Neutral,
    Radical,
    Opportunist
};

struct CharacterAttributes {
    float scheming;
    float charisma;
    float intelligence;
    float wealth;
    float military;
    float reputation;
    float integrity;
    float ambition;

    float GetAverage() const {
        return (scheming + charisma + intelligence + wealth + military + reputation + integrity + ambition) / 8.0f;
    }
    float GetPowerScore() const {
        return scheming * 0.25f + wealth * 0.2f + military * 0.15f + charisma * 0.15f + intelligence * 0.1f + reputation * 0.1f + ambition * 0.05f;
    }
    float GetTrustScore() const {
        return integrity * 0.4f + (100.0f - ambition) * 0.3f + (100.0f - scheming) * 0.3f;
    }
};

struct HiddenMotive {
    std::string personalGoal;
    std::string secretRelationship;
    float betrayalTendency;
    std::string triggerCondition;
    std::string hiddenAgenda;
    bool revealed;
};

struct RelationshipRecord {
    float favorability;
    float trust;
    float threatPerception;
    std::vector<std::string> interactionHistory;
};

class Character {
public:
    Character(const std::string& id, const std::string& name, PoliticalStance stance,
              const CharacterAttributes& attrs, const HiddenMotive& motive)
        : id_(id), name_(name), stance_(stance), attributes_(attrs), hiddenMotive_(motive) {}

    const std::string& GetId() const { return id_; }
    const std::string& GetName() const { return name_; }
    PoliticalStance GetStance() const { return stance_; }
    const CharacterAttributes& GetAttributes() const { return attributes_; }
    const HiddenMotive& GetHiddenMotive() const { return hiddenMotive_; }
    const std::string& GetFactionId() const { return factionId_; }
    bool IsAlive() const { return alive_; }
    bool IsActive() const { return active_; }
    float GetHealth() const { return health_; }

    void SetFactionId(const std::string& factionId) { factionId_ = factionId; }
    void SetAlive(bool alive) { alive_ = alive; }
    void SetActive(bool active) { active_ = active; }
    void ModifyHealth(float delta) { health_ = std::clamp(health_ + delta, 0.0f, 100.0f); }

    void ModifyAttribute(const std::string& attr, float delta);
    void RevealMotive() { hiddenMotive_.revealed = true; }

    float GetFavorability(const std::string& otherId) const;
    float GetTrust(const std::string& otherId) const;
    float GetThreatPerception(const std::string& otherId) const;
    void ModifyFavorability(const std::string& otherId, float delta);
    void ModifyTrust(const std::string& otherId, float delta);
    void ModifyThreatPerception(const std::string& otherId, float delta);
    void AddInteraction(const std::string& otherId, const std::string& interaction);
    const RelationshipRecord* GetRelationship(const std::string& otherId) const;

    bool ShouldBetray(const std::string& targetId) const;
    float CalculateDecisionWeight(const std::string& option, float shortTermGain, float longTermImpact, float risk) const;

private:
    std::string id_;
    std::string name_;
    PoliticalStance stance_;
    CharacterAttributes attributes_;
    HiddenMotive hiddenMotive_;
    std::string factionId_;
    bool alive_ = true;
    bool active_ = true;
    float health_ = 100.0f;
    std::unordered_map<std::string, RelationshipRecord> relationships_;
};

class CharacterAI {
public:
    enum class DecisionType {
        Support,
        Oppose,
        Neutrality,
        Betrayal,
        Alliance,
        Manipulation,
        SelfPreservation
    };

    struct AIDecision {
        DecisionType type;
        float confidence;
        std::string targetId;
        std::string reasoning;
        float weight;
    };

    AIDecision MakeDecision(const Character& character, const std::vector<AIDecision>& options) const;
    std::vector<AIDecision> GenerateOptions(const Character& character, int turn) const;
    float EvaluateStanceCompatibility(PoliticalStance a, PoliticalStance b) const;
    float EvaluateBetrayalUrgency(const Character& character, const std::string& targetId) const;
    DecisionType DetermineStanceAction(PoliticalStance stance, float situationUrgency) const;

private:
    float CalculateUtility(const Character& character, const AIDecision& option) const;
};

class RelationshipSystem {
public:
    void UpdateRelationship(const std::string& charA, const std::string& charB, float favorDelta, float trustDelta);
    float GetFavorability(const std::string& charA, const std::string& charB) const;
    float GetTrust(const std::string& charA, const std::string& charB) const;
    void ProcessTurnUpdate();
    std::vector<std::string> GetAllies(const std::string& characterId, float minFavorability) const;
    std::vector<std::string> GetEnemies(const std::string& characterId, float maxFavorability) const;
};

class CharacterManager : public IGameSystem {
public:
    CharacterManager() = default;

    void Initialize() override;
    void Reset() override;
    void OnTurnUpdate(int turn) override;
    void OnFrameUpdate(float deltaTime) override {}
    std::string GetSystemName() const override { return "CharacterManager"; }

    Character* CreateCharacter(const std::string& id, const std::string& name, PoliticalStance stance,
                                const CharacterAttributes& attrs, const HiddenMotive& motive);
    Character* GetCharacter(const std::string& characterId);
    const Character* GetCharacter(const std::string& characterId) const;
    const std::unordered_map<std::string, std::unique_ptr<Character>>& GetAllCharacters() const { return characters_; }

    std::vector<Character*> GetCharactersByStance(PoliticalStance stance) const;
    std::vector<Character*> GetCharactersByFaction(const std::string& factionId) const;
    std::vector<Character*> GetActiveCharacters() const;

    CharacterAI::AIDecision GetCharacterDecision(const std::string& characterId, int turn);
    std::vector<CharacterAI::AIDecision> GetCharacterOptions(const std::string& characterId, int turn);

    RelationshipSystem& GetRelationshipSystem() { return relationshipSystem_; }
    CharacterAI& GetAI() { return ai_; }

    using CharacterCallback = std::function<void(const Character&)>;
    void RegisterCallback(CharacterCallback cb) { callbacks_.push_back(cb); }

private:
    void InitializeDefaultCharacters();

    std::unordered_map<std::string, std::unique_ptr<Character>> characters_;
    CharacterAI ai_;
    RelationshipSystem relationshipSystem_;
    std::vector<CharacterCallback> callbacks_;
};

}
