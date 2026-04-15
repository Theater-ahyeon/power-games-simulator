#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <algorithm>
#include "../Core/GameSystem.h"
#include "../Core/EventBus.h"

namespace PowerGame {

enum class FactionIdeology {
    Conservative,
    Liberal,
    Neutral,
    Radical,
    Opportunist
};

struct FactionMember {
    std::string characterId;
    float loyalty;
    float contribution;
    std::string role;
};

class Faction {
public:
    Faction(const std::string& id, const std::string& name, FactionIdeology ideology)
        : id_(id), name_(name), ideology_(ideology) {}

    const std::string& GetId() const { return id_; }
    const std::string& GetName() const { return name_; }
    FactionIdeology GetIdeology() const { return ideology_; }

    float GetPower() const { return power_; }
    float GetCohesion() const { return cohesion_; }
    float GetInfluence() const { return influence_; }
    int GetMemberCount() const { return static_cast<int>(members_.size()); }
    bool IsActive() const { return active_; }

    void AddMember(const FactionMember& member);
    bool RemoveMember(const std::string& characterId);
    FactionMember* GetMember(const std::string& characterId);
    const std::vector<FactionMember>& GetMembers() const { return members_; }

    void ModifyPower(float delta) { power_ = std::clamp(power_ + delta, 0.0f, 100.0f); }
    void ModifyCohesion(float delta) { cohesion_ = std::clamp(cohesion_ + delta, 0.0f, 100.0f); }
    void ModifyInfluence(float delta) { influence_ = std::clamp(influence_ + delta, 0.0f, 100.0f); }

    void SetActive(bool active) { active_ = active; }
    void UpdateCohesion();
    void Reset();

private:
    std::string id_;
    std::string name_;
    FactionIdeology ideology_;
    float power_ = 10.0f;
    float cohesion_ = 50.0f;
    float influence_ = 10.0f;
    bool active_ = true;
    std::vector<FactionMember> members_;
};

struct TalentProfile {
    std::string characterId;
    std::string name;
    float skill;
    float potential;
    float cost;
    FactionIdeology preferredIdeology;
};

class LoyaltySystem {
public:
    struct LoyaltyRecord {
        std::string characterId;
        std::string factionId;
        float loyalty;
        float trust;
        float fear;
        float ambition;
    };

    void UpdateLoyalty(const std::string& charId, const std::string& factionId, float delta);
    void UpdateTrust(const std::string& charId, const std::string& factionId, float delta);
    void UpdateFear(const std::string& charId, const std::string& factionId, float delta);

    float GetLoyalty(const std::string& charId, const std::string& factionId) const;
    float GetTrust(const std::string& charId, const std::string& factionId) const;
    float GetDefectionRisk(const std::string& charId, const std::string& factionId) const;

    void ProcessTurnUpdate();

    using LoyaltyCallback = std::function<void(const std::string&, const std::string&, float)>;
    void RegisterCallback(LoyaltyCallback cb) { callbacks_.push_back(cb); }

private:
    std::string MakeKey(const std::string& charId, const std::string& factionId) const {
        return charId + ":" + factionId;
    }
    std::unordered_map<std::string, LoyaltyRecord> records_;
    std::vector<LoyaltyCallback> callbacks_;
};

class TalentRecruitment {
public:
    struct RecruitmentOffer {
        std::string targetId;
        std::string factionId;
        float offeredWealth;
        float offeredInfluence;
        float offeredPosition;
        float successChance;
    };

    float CalculateSuccessChance(const RecruitmentOffer& offer) const;
    bool ExecuteRecruitment(const RecruitmentOffer& offer);
    std::vector<TalentProfile> GetAvailableTalents() const;
    void GenerateNewTalents(int turn);

    void SetAvailableTalents(const std::vector<TalentProfile>& talents) { availableTalents_ = talents; }

private:
    std::vector<TalentProfile> availableTalents_;
};

class FactionManager : public IGameSystem {
public:
    FactionManager() = default;

    void Initialize() override;
    void Reset() override;
    void OnTurnUpdate(int turn) override;
    void OnFrameUpdate(float deltaTime) override {}
    std::string GetSystemName() const override { return "FactionManager"; }

    Faction* CreateFaction(const std::string& id, const std::string& name, FactionIdeology ideology);
    bool DestroyFaction(const std::string& factionId);
    Faction* GetFaction(const std::string& factionId);
    const std::unordered_map<std::string, std::unique_ptr<Faction>>& GetAllFactions() const { return factions_; }

    LoyaltySystem& GetLoyaltySystem() { return loyaltySystem_; }
    TalentRecruitment& GetTalentRecruitment() { return talentRecruitment_; }

    std::vector<Faction*> GetFactionsByIdeology(FactionIdeology ideology) const;
    float GetFactionBalance() const;
    std::string GetDominantFaction() const;

private:
    std::unordered_map<std::string, std::unique_ptr<Faction>> factions_;
    LoyaltySystem loyaltySystem_;
    TalentRecruitment talentRecruitment_;
};

}
