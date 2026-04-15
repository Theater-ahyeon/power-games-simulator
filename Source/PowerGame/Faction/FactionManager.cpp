#include "FactionManager.h"
#include <algorithm>
#include <random>

namespace PowerGame {

void Faction::AddMember(const FactionMember& member) {
    auto it = std::find_if(members_.begin(), members_.end(),
        [&member](const FactionMember& m) { return m.characterId == member.characterId; });
    if (it == members_.end()) {
        members_.push_back(member);
        power_ += member.contribution * 0.5f;
    }
}

bool Faction::RemoveMember(const std::string& characterId) {
    auto it = std::find_if(members_.begin(), members_.end(),
        [&characterId](const FactionMember& m) { return m.characterId == characterId; });
    if (it != members_.end()) {
        power_ -= it->contribution * 0.5f;
        power_ = std::max(0.0f, power_);
        members_.erase(it);
        return true;
    }
    return false;
}

FactionMember* Faction::GetMember(const std::string& characterId) {
    auto it = std::find_if(members_.begin(), members_.end(),
        [&characterId](const FactionMember& m) { return m.characterId == characterId; });
    return it != members_.end() ? &(*it) : nullptr;
}

void Faction::UpdateCohesion() {
    if (members_.empty()) {
        cohesion_ = 0.0f;
        return;
    }
    float totalLoyalty = 0.0f;
    for (const auto& member : members_) {
        totalLoyalty += member.loyalty;
    }
    cohesion_ = totalLoyalty / static_cast<float>(members_.size());
}

void Faction::Reset() {
    power_ = 10.0f;
    cohesion_ = 50.0f;
    influence_ = 10.0f;
    active_ = true;
    members_.clear();
}

void LoyaltySystem::UpdateLoyalty(const std::string& charId, const std::string& factionId, float delta) {
    std::string key = MakeKey(charId, factionId);
    auto& record = records_[key];
    record.characterId = charId;
    record.factionId = factionId;
    record.loyalty = std::clamp(record.loyalty + delta, 0.0f, 100.0f);
    for (auto& cb : callbacks_) {
        cb(charId, factionId, record.loyalty);
    }
}

void LoyaltySystem::UpdateTrust(const std::string& charId, const std::string& factionId, float delta) {
    std::string key = MakeKey(charId, factionId);
    auto& record = records_[key];
    record.characterId = charId;
    record.factionId = factionId;
    record.trust = std::clamp(record.trust + delta, 0.0f, 100.0f);
}

void LoyaltySystem::UpdateFear(const std::string& charId, const std::string& factionId, float delta) {
    std::string key = MakeKey(charId, factionId);
    auto& record = records_[key];
    record.characterId = charId;
    record.factionId = factionId;
    record.fear = std::clamp(record.fear + delta, 0.0f, 100.0f);
}

float LoyaltySystem::GetLoyalty(const std::string& charId, const std::string& factionId) const {
    auto it = records_.find(MakeKey(charId, factionId));
    return it != records_.end() ? it->second.loyalty : 50.0f;
}

float LoyaltySystem::GetTrust(const std::string& charId, const std::string& factionId) const {
    auto it = records_.find(MakeKey(charId, factionId));
    return it != records_.end() ? it->second.trust : 50.0f;
}

float LoyaltySystem::GetDefectionRisk(const std::string& charId, const std::string& factionId) const {
    float loyalty = GetLoyalty(charId, factionId);
    float trust = GetTrust(charId, factionId);
    auto it = records_.find(MakeKey(charId, factionId));
    float ambition = it != records_.end() ? it->second.ambition : 50.0f;
    float risk = (100.0f - loyalty) * 0.4f + (100.0f - trust) * 0.3f + ambition * 0.3f;
    return std::clamp(risk, 0.0f, 100.0f);
}

void LoyaltySystem::ProcessTurnUpdate() {
    for (auto& [key, record] : records_) {
        record.loyalty += (record.trust * 0.02f - 1.0f);
        record.loyalty = std::clamp(record.loyalty, 0.0f, 100.0f);
        if (record.fear > 60.0f) {
            record.loyalty += 1.0f;
            record.trust -= 0.5f;
        }
        record.trust = std::clamp(record.trust, 0.0f, 100.0f);
    }
}

float TalentRecruitment::CalculateSuccessChance(const RecruitmentOffer& offer) const {
    float base = 0.3f;
    base += offer.offeredWealth * 0.002f;
    base += offer.offeredInfluence * 0.003f;
    base += offer.offeredPosition * 0.004f;
    return std::clamp(base, 0.05f, 0.95f);
}

bool TalentRecruitment::ExecuteRecruitment(const RecruitmentOffer& offer) {
    float chance = CalculateSuccessChance(offer);
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng) < chance;
}

std::vector<TalentProfile> TalentRecruitment::GetAvailableTalents() const {
    return availableTalents_;
}

void TalentRecruitment::GenerateNewTalents(int turn) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> skillDist(20.0f, 80.0f);
    std::uniform_real_distribution<float> potentialDist(30.0f, 90.0f);
    std::uniform_real_distribution<float> costDist(10.0f, 50.0f);
    std::uniform_int_distribution<int> ideologyDist(0, 4);

    int count = 1 + (turn % 3);
    for (int i = 0; i < count; i++) {
        TalentProfile talent;
        talent.characterId = "talent_" + std::to_string(turn) + "_" + std::to_string(i);
        talent.name = "Recruit_" + std::to_string(turn) + "_" + std::to_string(i);
        talent.skill = skillDist(rng);
        talent.potential = potentialDist(rng);
        talent.cost = costDist(rng);
        talent.preferredIdeology = static_cast<FactionIdeology>(ideologyDist(rng));
        availableTalents_.push_back(talent);
    }
}

void FactionManager::Initialize() {
    CreateFaction("conservatives", "Conservative Alliance", FactionIdeology::Conservative);
    CreateFaction("liberals", "Liberal Coalition", FactionIdeology::Liberal);
    CreateFaction("neutrals", "Central Bloc", FactionIdeology::Neutral);
    CreateFaction("radicals", "Radical Front", FactionIdeology::Radical);
    CreateFaction("opportunists", "Opportunist Circle", FactionIdeology::Opportunist);
}

void FactionManager::Reset() {
    for (auto& [id, faction] : factions_) {
        faction->Reset();
    }
}

void FactionManager::OnTurnUpdate(int turn) {
    for (auto& [id, faction] : factions_) {
        if (!faction->IsActive()) continue;
        faction->UpdateCohesion();
        faction->ModifyInfluence(faction->GetCohesion() * 0.01f);
    }
    loyaltySystem_.ProcessTurnUpdate();
    if (turn % 3 == 0) {
        talentRecruitment_.GenerateNewTalents(turn);
    }
}

Faction* FactionManager::CreateFaction(const std::string& id, const std::string& name, FactionIdeology ideology) {
    auto faction = std::make_unique<Faction>(id, name, ideology);
    auto* ptr = faction.get();
    factions_[id] = std::move(faction);
    EventBus::Instance().Publish(Events::FactionCreated, FactionEvent{id, name});
    return ptr;
}

bool FactionManager::DestroyFaction(const std::string& factionId) {
    auto it = factions_.find(factionId);
    if (it == factions_.end()) return false;
    it->second->SetActive(false);
    EventBus::Instance().Publish(Events::FactionDestroyed, FactionEvent{factionId, ""});
    return true;
}

Faction* FactionManager::GetFaction(const std::string& factionId) {
    auto it = factions_.find(factionId);
    return it != factions_.end() ? it->second.get() : nullptr;
}

std::vector<Faction*> FactionManager::GetFactionsByIdeology(FactionIdeology ideology) const {
    std::vector<Faction*> result;
    for (const auto& [id, faction] : factions_) {
        if (faction->GetIdeology() == ideology && faction->IsActive()) {
            result.push_back(faction.get());
        }
    }
    return result;
}

float FactionManager::GetFactionBalance() const {
    float totalPower = 0.0f;
    float maxPower = 0.0f;
    for (const auto& [id, faction] : factions_) {
        if (!faction->IsActive()) continue;
        totalPower += faction->GetPower();
        maxPower = std::max(maxPower, faction->GetPower());
    }
    if (totalPower == 0.0f) return 1.0f;
    return 1.0f - (maxPower / totalPower);
}

std::string FactionManager::GetDominantFaction() const {
    std::string dominant;
    float maxPower = 0.0f;
    for (const auto& [id, faction] : factions_) {
        if (faction->IsActive() && faction->GetPower() > maxPower) {
            maxPower = faction->GetPower();
            dominant = id;
        }
    }
    return dominant;
}

}
