#include "CharacterManager.h"
#include <algorithm>
#include <random>

namespace PowerGame {

void Character::ModifyAttribute(const std::string& attr, float delta) {
    if (attr == "scheming") attributes_.scheming = std::clamp(attributes_.scheming + delta, 0.0f, 100.0f);
    else if (attr == "charisma") attributes_.charisma = std::clamp(attributes_.charisma + delta, 0.0f, 100.0f);
    else if (attr == "intelligence") attributes_.intelligence = std::clamp(attributes_.intelligence + delta, 0.0f, 100.0f);
    else if (attr == "wealth") attributes_.wealth = std::clamp(attributes_.wealth + delta, 0.0f, 100.0f);
    else if (attr == "military") attributes_.military = std::clamp(attributes_.military + delta, 0.0f, 100.0f);
    else if (attr == "reputation") attributes_.reputation = std::clamp(attributes_.reputation + delta, 0.0f, 100.0f);
    else if (attr == "integrity") attributes_.integrity = std::clamp(attributes_.integrity + delta, 0.0f, 100.0f);
    else if (attr == "ambition") attributes_.ambition = std::clamp(attributes_.ambition + delta, 0.0f, 100.0f);
}

float Character::GetFavorability(const std::string& otherId) const {
    auto it = relationships_.find(otherId);
    return it != relationships_.end() ? it->second.favorability : 50.0f;
}

float Character::GetTrust(const std::string& otherId) const {
    auto it = relationships_.find(otherId);
    return it != relationships_.end() ? it->second.trust : 50.0f;
}

float Character::GetThreatPerception(const std::string& otherId) const {
    auto it = relationships_.find(otherId);
    return it != relationships_.end() ? it->second.threatPerception : 30.0f;
}

void Character::ModifyFavorability(const std::string& otherId, float delta) {
    auto& rel = relationships_[otherId];
    rel.favorability = std::clamp(rel.favorability + delta, 0.0f, 100.0f);
}

void Character::ModifyTrust(const std::string& otherId, float delta) {
    auto& rel = relationships_[otherId];
    rel.trust = std::clamp(rel.trust + delta, 0.0f, 100.0f);
}

void Character::ModifyThreatPerception(const std::string& otherId, float delta) {
    auto& rel = relationships_[otherId];
    rel.threatPerception = std::clamp(rel.threatPerception + delta, 0.0f, 100.0f);
}

void Character::AddInteraction(const std::string& otherId, const std::string& interaction) {
    auto& rel = relationships_[otherId];
    rel.interactionHistory.push_back(interaction);
}

const RelationshipRecord* Character::GetRelationship(const std::string& otherId) const {
    auto it = relationships_.find(otherId);
    return it != relationships_.end() ? &it->second : nullptr;
}

bool Character::ShouldBetray(const std::string& targetId) const {
    float trust = GetTrust(targetId);
    float favorability = GetFavorability(targetId);
    float betrayalScore = hiddenMotive_.betrayalTendency * 0.4f +
                          attributes_.ambition * 0.3f +
                          (100.0f - trust) * 0.2f +
                          (100.0f - favorability) * 0.1f;
    return betrayalScore > 60.0f;
}

float Character::CalculateDecisionWeight(const std::string& option, float shortTermGain,
                                           float longTermImpact, float risk) const {
    float weight = shortTermGain * 0.3f + longTermImpact * 0.4f;
    weight -= risk * (attributes_.scheming > 50.0f ? 0.2f : 0.4f);
    weight += attributes_.ambition * 0.01f;
    if (hiddenMotive_.personalGoal.find(option) != std::string::npos) {
        weight += 20.0f;
    }
    return weight;
}

CharacterAI::AIDecision CharacterAI::MakeDecision(const Character& character,
                                                     const std::vector<AIDecision>& options) const {
    if (options.empty()) {
        return {DecisionType::Neutrality, 0.5f, "", "No options available", 0.0f};
    }

    AIDecision bestDecision = options[0];
    float bestUtility = CalculateUtility(character, options[0]);

    for (size_t i = 1; i < options.size(); i++) {
        float utility = CalculateUtility(character, options[i]);
        if (utility > bestUtility) {
            bestUtility = utility;
            bestDecision = options[i];
        }
    }

    bestDecision.confidence = std::clamp(bestUtility / 100.0f, 0.0f, 1.0f);
    return bestDecision;
}

std::vector<CharacterAI::AIDecision> CharacterAI::GenerateOptions(const Character& character, int turn) const {
    std::vector<AIDecision> options;
    const auto& attrs = character.GetAttributes();

    options.push_back({DecisionType::Support, 0.5f, "", "Support current power structure", 30.0f});
    options.push_back({DecisionType::Oppose, 0.5f, "", "Oppose current power structure", 20.0f});
    options.push_back({DecisionType::Neutrality, 0.7f, "", "Maintain neutrality", 40.0f});

    if (attrs.ambition > 60.0f) {
        options.push_back({DecisionType::Manipulation, 0.4f, "", "Manipulate others for gain", 25.0f});
    }
    if (attrs.scheming > 50.0f && character.GetHiddenMotive().betrayalTendency > 40.0f) {
        options.push_back({DecisionType::Betrayal, 0.3f, "", "Betray current ally", 15.0f});
    }
    if (attrs.wealth > 40.0f || attrs.military > 40.0f) {
        options.push_back({DecisionType::Alliance, 0.6f, "", "Form strategic alliance", 35.0f});
    }
    if (character.GetHealth() < 50.0f || attrs.reputation < 30.0f) {
        options.push_back({DecisionType::SelfPreservation, 0.8f, "", "Focus on self-preservation", 45.0f});
    }

    return options;
}

float CharacterAI::EvaluateStanceCompatibility(PoliticalStance a, PoliticalStance b) const {
    if (a == b) return 1.0f;
    if (a == PoliticalStance::Neutral || b == PoliticalStance::Neutral) return 0.6f;
    if (a == PoliticalStance::Opportunist || b == PoliticalStance::Opportunist) return 0.5f;

    bool aExtreme = (a == PoliticalStance::Conservative || a == PoliticalStance::Radical);
    bool bExtreme = (b == PoliticalStance::Conservative || b == PoliticalStance::Radical);

    if (aExtreme && bExtreme) {
        if (a != b) return 0.1f;
    }
    if (a == PoliticalStance::Liberal && b == PoliticalStance::Radical) return 0.4f;
    if (a == PoliticalStance::Conservative && b == PoliticalStance::Liberal) return 0.3f;

    return 0.3f;
}

float CharacterAI::EvaluateBetrayalUrgency(const Character& character, const std::string& targetId) const {
    const auto& motive = character.GetHiddenMotive();
    const auto& attrs = character.GetAttributes();
    float trust = character.GetTrust(targetId);
    float favorability = character.GetFavorability(targetId);

    float urgency = motive.betrayalTendency * 0.3f +
                    attrs.ambition * 0.25f +
                    (100.0f - trust) * 0.25f +
                    (100.0f - favorability) * 0.2f;
    return std::clamp(urgency, 0.0f, 100.0f);
}

CharacterAI::DecisionType CharacterAI::DetermineStanceAction(PoliticalStance stance, float situationUrgency) const {
    switch (stance) {
        case PoliticalStance::Conservative:
            return situationUrgency > 70.0f ? DecisionType::Oppose : DecisionType::Support;
        case PoliticalStance::Liberal:
            return situationUrgency > 50.0f ? DecisionType::Oppose : DecisionType::Neutrality;
        case PoliticalStance::Neutral:
            return DecisionType::Neutrality;
        case PoliticalStance::Radical:
            return situationUrgency > 30.0f ? DecisionType::Oppose : DecisionType::Manipulation;
        case PoliticalStance::Opportunist:
            return situationUrgency > 60.0f ? DecisionType::Betrayal : DecisionType::Manipulation;
        default:
            return DecisionType::Neutrality;
    }
}

float CharacterAI::CalculateUtility(const Character& character, const AIDecision& option) const {
    const auto& attrs = character.GetAttributes();
    float utility = option.weight;

    switch (option.type) {
        case DecisionType::Support:
            utility += attrs.integrity * 0.2f + (100.0f - attrs.ambition) * 0.1f;
            break;
        case DecisionType::Oppose:
            utility += attrs.ambition * 0.2f + attrs.scheming * 0.1f;
            break;
        case DecisionType::Neutrality:
            utility += (100.0f - attrs.ambition) * 0.15f + attrs.intelligence * 0.1f;
            break;
        case DecisionType::Betrayal:
            utility += attrs.scheming * 0.3f + attrs.ambition * 0.2f - attrs.integrity * 0.2f;
            break;
        case DecisionType::Alliance:
            utility += attrs.charisma * 0.2f + attrs.intelligence * 0.1f;
            break;
        case DecisionType::Manipulation:
            utility += attrs.scheming * 0.25f + attrs.intelligence * 0.15f - attrs.integrity * 0.1f;
            break;
        case DecisionType::SelfPreservation:
            utility += (100.0f - character.GetHealth()) * 0.2f;
            break;
    }

    return utility;
}

void RelationshipSystem::UpdateRelationship(const std::string& charA, const std::string& charB,
                                              float favorDelta, float trustDelta) {
}

float RelationshipSystem::GetFavorability(const std::string& charA, const std::string& charB) const {
    return 50.0f;
}

float RelationshipSystem::GetTrust(const std::string& charA, const std::string& charB) const {
    return 50.0f;
}

void RelationshipSystem::ProcessTurnUpdate() {
}

std::vector<std::string> RelationshipSystem::GetAllies(const std::string& characterId, float minFavorability) const {
    return {};
}

std::vector<std::string> RelationshipSystem::GetEnemies(const std::string& characterId, float maxFavorability) const {
    return {};
}

void CharacterManager::Initialize() {
    InitializeDefaultCharacters();
}

void CharacterManager::InitializeDefaultCharacters() {
    struct CharDef {
        std::string id, name;
        PoliticalStance stance;
        CharacterAttributes attrs;
        HiddenMotive motive;
        std::string factionId;
    };

    std::vector<CharDef> defs = {
        {"chen_weiming", "Chen Weiming", PoliticalStance::Conservative,
         {85, 70, 75, 90, 60, 80, 40, 75},
         {"Maintain traditional power structure", "Secret deal with military", 30.0f, "When military support wavers", "Consolidate party control", false},
         "conservatives"},
        {"liu_meihua", "Liu Meihua", PoliticalStance::Conservative,
         {70, 85, 65, 75, 30, 85, 55, 60},
         {"Preserve family political dynasty", "Related to former president", 20.0f, "When family threatened", "Ensure succession", false},
         "conservatives"},
        {"wang_zhenbang", "Wang Zhenbang", PoliticalStance::Conservative,
         {80, 55, 70, 85, 70, 65, 35, 80},
         {"Control economic policy", "Owns major corporation", 45.0f, "When profits threatened", "Monopolize key industries", false},
         "conservatives"},
        {"zhao_xueren", "Zhao Xueren", PoliticalStance::Conservative,
         {65, 60, 80, 60, 50, 75, 50, 55},
         {"Maintain bureaucratic influence", "Former classmate of president", 25.0f, "When reform threatens position", "Block reform initiatives", false},
         "conservatives"},
        {"zhang_haoran", "Zhang Haoran", PoliticalStance::Liberal,
         {60, 80, 85, 50, 25, 90, 80, 70},
         {"Push democratic reform", "Connected to foreign NGOs", 15.0f, "When reform window opens", "Constitutional reform", false},
         "liberals"},
        {"sun_liying", "Sun Liying", PoliticalStance::Liberal,
         {55, 75, 80, 40, 20, 85, 85, 65},
         {"Protect civil liberties", "Whistleblower network member", 10.0f, "When rights violated", "Transparency legislation", false},
         "liberals"},
        {"huang_jianguo", "Huang Jianguo", PoliticalStance::Liberal,
         {70, 65, 75, 55, 30, 70, 70, 60},
         {"Reform judicial system", "Has evidence of corruption", 25.0f, "When evidence is strong enough", "Judicial independence", false},
         "liberals"},
        {"xu_weilan", "Xu Weilan", PoliticalStance::Liberal,
         {50, 70, 90, 35, 15, 80, 90, 50},
         {"Academic freedom advocate", "Published critical research", 5.0f, "Never - committed to truth", "Educational reform", false},
         "liberals"},
        {"zhou_tianping", "Zhou Tianping", PoliticalStance::Neutral,
         {75, 60, 85, 70, 45, 70, 65, 45},
         {"Maintain balance of power", "Mediator between factions", 20.0f, "When one faction dominates", "Broker peace", false},
         "neutrals"},
        {"ma_shufen", "Ma Shufen", PoliticalStance::Neutral,
         {60, 70, 70, 65, 35, 75, 70, 40},
         {"Economic stability", "Central bank connections", 15.0f, "When economy destabilizes", "Financial reform", false},
         "neutrals"},
        {"yang_guangming", "Yang Guangming", PoliticalStance::Neutral,
         {65, 55, 80, 80, 50, 60, 55, 50},
         {"Personal enrichment", "Business empire spanning factions", 35.0f, "When opportunity arises", "Expand business influence", false},
         "neutrals"},
        {"feng_daluo", "Feng Daluo", PoliticalStance::Neutral,
         {70, 50, 75, 60, 55, 55, 60, 55},
         {"Military modernization", "Veteran of border conflicts", 25.0f, "When national security at risk", "Strengthen armed forces", false},
         "neutrals"},
        {"lin_zhaoyi", "Lin Zhaoyi", PoliticalStance::Radical,
         {80, 75, 70, 40, 45, 55, 30, 90},
         {"Overthrow existing order", "Underground movement leader", 60.0f, "When power vacuum emerges", "Revolutionary takeover", false},
         "radicals"},
        {"wu_zhenxiong", "Wu Zhenxiong", PoliticalStance::Radical,
         {85, 60, 65, 35, 65, 40, 25, 95},
         {"Military coup", "Controls paramilitary force", 70.0f, "When civilian government weak", "Establish military rule", false},
         "radicals"},
        {"qian_yanfei", "Qian Yanfei", PoliticalStance::Radical,
         {75, 80, 75, 30, 35, 60, 35, 85},
         {"Social revolution", "Charismatic protest leader", 50.0f, "When public anger peaks", "Mass mobilization", false},
         "radicals"},
        {"tang_ruihua", "Tang Ruihua", PoliticalStance::Radical,
         {70, 65, 80, 45, 40, 50, 40, 80},
         {"Technocratic revolution", "Silicon Valley connections", 40.0f, "When technology enables control", "Digital authoritarianism", false},
         "radicals"},
        {"he_shengli", "He Shengli", PoliticalStance::Opportunist,
         {90, 70, 75, 75, 50, 45, 20, 85},
         {"Personal power maximization", "Multiple faction memberships", 65.0f, "When power balance shifts", "Become kingmaker", false},
         "opportunists"},
        {"deng_xiaowei", "Deng Xiaowei", PoliticalStance::Opportunist,
         {80, 85, 70, 65, 30, 65, 30, 80},
         {"Media empire control", "Owns major news network", 55.0f, "When narrative control is possible", "Information dominance", false},
         "opportunists"},
        {"luo_jiashi", "Luo Jiashi", PoliticalStance::Opportunist,
         {85, 60, 80, 80, 40, 50, 25, 90},
         {"Wealth accumulation", "Offshore accounts network", 50.0f, "When financial opportunity arises", "Economic empire", false},
         "opportunists"},
        {"cai_hongyu", "Cai Hongyu", PoliticalStance::Opportunist,
         {75, 75, 70, 55, 45, 60, 35, 75},
         {"Political kingmaker", "Blackmail material on elites", 60.0f, "When leverage is sufficient", "Control from shadows", false},
         "opportunists"},
        {"song_qingling", "Song Qingling", PoliticalStance::Neutral,
         {55, 80, 85, 50, 20, 90, 85, 35},
         {"Social welfare reform", "Grassroots organization leader", 10.0f, "When people suffer", "Poverty alleviation", false},
         "neutrals"},
        {"wei_donghai", "Wei Donghai", PoliticalStance::Opportunist,
         {70, 65, 60, 90, 55, 40, 15, 80},
         {"Control energy sector", "Oil industry connections", 45.0f, "When energy crisis looms", "Energy monopoly", false},
         "opportunists"},
    };

    for (const auto& def : defs) {
        auto* character = CreateCharacter(def.id, def.name, def.stance, def.attrs, def.motive);
        if (character) {
            character->SetFactionId(def.factionId);
        }
    }
}

void CharacterManager::Reset() {
    for (auto& [id, character] : characters_) {
        character->SetAlive(true);
        character->SetActive(true);
        character->ModifyHealth(100.0f - character->GetHealth());
    }
}

void CharacterManager::OnTurnUpdate(int turn) {
    for (auto& [id, character] : characters_) {
        if (!character->IsActive() || !character->IsAlive()) continue;

        const auto& attrs = character->GetAttributes();
        float healthDelta = -0.5f + attrs.wealth * 0.005f;
        character->ModifyHealth(healthDelta);

        if (character->GetHealth() <= 0) {
            character->SetAlive(false);
            character->SetActive(false);
            EventBus::Instance().Publish(Events::CharacterDefected, CharacterEvent{
                id, "Died", "Health depleted"
            });
        }
    }
}

Character* CharacterManager::CreateCharacter(const std::string& id, const std::string& name,
                                               PoliticalStance stance, const CharacterAttributes& attrs,
                                               const HiddenMotive& motive) {
    auto character = std::make_unique<Character>(id, name, stance, attrs, motive);
    auto* ptr = character.get();
    characters_[id] = std::move(character);
    return ptr;
}

Character* CharacterManager::GetCharacter(const std::string& characterId) {
    auto it = characters_.find(characterId);
    return it != characters_.end() ? it->second.get() : nullptr;
}

const Character* CharacterManager::GetCharacter(const std::string& characterId) const {
    auto it = characters_.find(characterId);
    return it != characters_.end() ? it->second.get() : nullptr;
}

std::vector<Character*> CharacterManager::GetCharactersByStance(PoliticalStance stance) const {
    std::vector<Character*> result;
    for (const auto& [id, character] : characters_) {
        if (character->GetStance() == stance && character->IsActive()) {
            result.push_back(character.get());
        }
    }
    return result;
}

std::vector<Character*> CharacterManager::GetCharactersByFaction(const std::string& factionId) const {
    std::vector<Character*> result;
    for (const auto& [id, character] : characters_) {
        if (character->GetFactionId() == factionId && character->IsActive()) {
            result.push_back(character.get());
        }
    }
    return result;
}

std::vector<Character*> CharacterManager::GetActiveCharacters() const {
    std::vector<Character*> result;
    for (const auto& [id, character] : characters_) {
        if (character->IsActive() && character->IsAlive()) {
            result.push_back(character.get());
        }
    }
    return result;
}

CharacterAI::AIDecision CharacterManager::GetCharacterDecision(const std::string& characterId, int turn) {
    auto* character = GetCharacter(characterId);
    if (!character) return {CharacterAI::DecisionType::Neutrality, 0.0f, "", "Character not found", 0.0f};
    auto options = ai_.GenerateOptions(*character, turn);
    return ai_.MakeDecision(*character, options);
}

std::vector<CharacterAI::AIDecision> CharacterManager::GetCharacterOptions(const std::string& characterId, int turn) {
    auto* character = GetCharacter(characterId);
    if (!character) return {};
    return ai_.GenerateOptions(*character, turn);
}

}
