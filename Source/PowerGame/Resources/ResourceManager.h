#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <cmath>
#include "../Core/GameSystem.h"
#include "../Core/EventBus.h"

namespace PowerGame {

enum class ResourceType {
    PoliticalCapital,
    Influence,
    Information,
    Wealth,
    Military,
    Reputation,
    Legitimacy,
    PublicSupport
};

inline std::string ResourceTypeToString(ResourceType type) {
    switch (type) {
        case ResourceType::PoliticalCapital: return "PoliticalCapital";
        case ResourceType::Influence: return "Influence";
        case ResourceType::Information: return "Information";
        case ResourceType::Wealth: return "Wealth";
        case ResourceType::Military: return "Military";
        case ResourceType::Reputation: return "Reputation";
        case ResourceType::Legitimacy: return "Legitimacy";
        case ResourceType::PublicSupport: return "PublicSupport";
        default: return "Unknown";
    }
}

struct ResourceConfig {
    ResourceType type;
    float initialValue;
    float maxValue;
    float regenRate;
};

struct ResourceConversion {
    ResourceType from;
    float fromAmount;
    ResourceType to;
    float toAmount;
    float efficiency;
    float GetRatio() const { return (toAmount * efficiency) / fromAmount; }
};

struct DelayedResourceEffect {
    ResourceType type;
    float amount;
    int triggerTurn;
    std::string reason;
};

class GameResource {
public:
    GameResource(ResourceType type, float initial, float max, float regenRate)
        : type_(type), current_(initial), baseValue_(initial),
          maxValue_(max), regenRate_(regenRate) {}

    bool Modify(float amount) {
        float newValue = current_ + amount;
        if (newValue < 0) return false;
        current_ = std::min(newValue, maxValue_);
        return true;
    }

    void Regenerate() {
        current_ = std::min(current_ + regenRate_, maxValue_);
    }

    void SetRegenRate(float rate) { regenRate_ = std::max(0.0f, rate); }
    void ModifyRegenRate(float delta) { regenRate_ = std::max(0.0f, regenRate_ + delta); }
    void Reset() { current_ = baseValue_; }

    ResourceType GetType() const { return type_; }
    float GetCurrent() const { return current_; }
    float GetMaxValue() const { return maxValue_; }
    float GetRegenRate() const { return regenRate_; }
    float GetRatio() const { return current_ / maxValue_; }

private:
    ResourceType type_;
    float current_;
    float baseValue_;
    float maxValue_;
    float regenRate_;
};

class ResourceManager : public IGameSystem {
public:
    ResourceManager() = default;

    void Initialize() override;
    void Reset() override;
    void OnTurnUpdate(int turn) override;
    void OnFrameUpdate(float deltaTime) override {}
    std::string GetSystemName() const override { return "ResourceManager"; }

    float GetResource(ResourceType type) const;
    bool ModifyResource(ResourceType type, float amount, const std::string& reason = "");
    bool ConvertResource(ResourceType from, ResourceType to, float amount);
    void AddDelayedEffect(ResourceType type, float amount, int delayTurns, const std::string& reason = "");

    std::unordered_map<ResourceType, float> GetAllResources() const;
    std::unordered_map<ResourceType, float> GetRegenRates() const;

    using ResourceChangeCallback = std::function<void(ResourceType, float, float)>;
    void RegisterChangeCallback(ResourceChangeCallback cb) { changeCallbacks_.push_back(cb); }

private:
    void InitializeConversions();
    void ProcessDelayedEffects(int currentTurn);

    std::unordered_map<ResourceType, GameResource> resources_;
    std::vector<ResourceConversion> conversions_;
    std::vector<DelayedResourceEffect> delayedEffects_;
    std::vector<ResourceChangeCallback> changeCallbacks_;
};

}
