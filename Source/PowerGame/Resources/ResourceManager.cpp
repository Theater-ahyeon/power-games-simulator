#include "ResourceManager.h"
#include "../Core/GameManager.h"
#include <algorithm>

namespace PowerGame {

void ResourceManager::Initialize() {
    resources_.emplace(ResourceType::PoliticalCapital,
        GameResource(ResourceType::PoliticalCapital, 50.0f, 100.0f, 2.0f));
    resources_.emplace(ResourceType::Influence,
        GameResource(ResourceType::Influence, 30.0f, 100.0f, 1.5f));
    resources_.emplace(ResourceType::Information,
        GameResource(ResourceType::Information, 20.0f, 100.0f, 1.0f));
    resources_.emplace(ResourceType::Wealth,
        GameResource(ResourceType::Wealth, 40.0f, 100.0f, 2.0f));
    resources_.emplace(ResourceType::Military,
        GameResource(ResourceType::Military, 10.0f, 100.0f, 0.5f));
    resources_.emplace(ResourceType::Reputation,
        GameResource(ResourceType::Reputation, 50.0f, 100.0f, 0.0f));
    resources_.emplace(ResourceType::Legitimacy,
        GameResource(ResourceType::Legitimacy, 60.0f, 100.0f, 0.0f));
    resources_.emplace(ResourceType::PublicSupport,
        GameResource(ResourceType::PublicSupport, 45.0f, 100.0f, 1.0f));

    InitializeConversions();
}

void ResourceManager::InitializeConversions() {
    conversions_ = {
        {ResourceType::Wealth, 20.0f, ResourceType::PoliticalCapital, 10.0f, 0.8f},
        {ResourceType::Information, 15.0f, ResourceType::Influence, 10.0f, 0.9f},
        {ResourceType::PoliticalCapital, 15.0f, ResourceType::Influence, 12.0f, 0.85f},
        {ResourceType::Wealth, 25.0f, ResourceType::Information, 15.0f, 0.7f},
        {ResourceType::Influence, 20.0f, ResourceType::Reputation, 8.0f, 0.75f},
        {ResourceType::PoliticalCapital, 30.0f, ResourceType::Legitimacy, 15.0f, 0.6f},
        {ResourceType::Wealth, 30.0f, ResourceType::Military, 12.0f, 0.65f},
        {ResourceType::PublicSupport, 20.0f, ResourceType::Legitimacy, 10.0f, 0.8f},
    };
}

void ResourceManager::Reset() {
    for (auto& [type, resource] : resources_) {
        resource.Reset();
    }
    delayedEffects_.clear();
}

void ResourceManager::OnTurnUpdate(int turn) {
    for (auto& [type, resource] : resources_) {
        float oldValue = resource.GetCurrent();
        resource.Regenerate();
        if (std::abs(resource.GetCurrent() - oldValue) > 0.01f) {
            for (auto& cb : changeCallbacks_) {
                cb(type, oldValue, resource.GetCurrent());
            }
        }
    }
    ProcessDelayedEffects(turn);
}

float ResourceManager::GetResource(ResourceType type) const {
    auto it = resources_.find(type);
    return it != resources_.end() ? it->second.GetCurrent() : 0.0f;
}

bool ResourceManager::ModifyResource(ResourceType type, float amount, const std::string& reason) {
    auto it = resources_.find(type);
    if (it == resources_.end()) return false;

    float oldValue = it->second.GetCurrent();
    if (!it->second.Modify(amount)) return false;

    for (auto& cb : changeCallbacks_) {
        cb(type, oldValue, it->second.GetCurrent());
    }

    ResourceChangeEvent evt;
    evt.type = static_cast<ResourceChangeEvent::Type>(type);
    evt.oldValue = oldValue;
    evt.newValue = it->second.GetCurrent();
    evt.reason = reason;
    EventBus::Instance().Publish(Events::ResourceChanged, evt);

    return true;
}

bool ResourceManager::ConvertResource(ResourceType from, ResourceType to, float amount) {
    auto it = std::find_if(conversions_.begin(), conversions_.end(),
        [from, to](const ResourceConversion& c) { return c.from == from && c.to == to; });
    if (it == conversions_.end()) return false;

    float cost = amount / it->GetRatio();
    if (GetResource(from) < cost) return false;

    ModifyResource(from, -cost, "Convert to " + ResourceTypeToString(to));
    ModifyResource(to, amount, "Convert from " + ResourceTypeToString(from));
    return true;
}

void ResourceManager::AddDelayedEffect(ResourceType type, float amount, int delayTurns, const std::string& reason) {
    delayedEffects_.push_back({
        type, amount,
        GameManager::Instance().GetCurrentTurn() + delayTurns,
        reason
    });
}

void ResourceManager::ProcessDelayedEffects(int currentTurn) {
    std::vector<DelayedResourceEffect> remaining;
    for (auto& effect : delayedEffects_) {
        if (effect.triggerTurn <= currentTurn) {
            ModifyResource(effect.type, effect.amount, effect.reason);
        } else {
            remaining.push_back(std::move(effect));
        }
    }
    delayedEffects_ = std::move(remaining);
}

std::unordered_map<ResourceType, float> ResourceManager::GetAllResources() const {
    std::unordered_map<ResourceType, float> result;
    for (const auto& [type, resource] : resources_) {
        result[type] = resource.GetCurrent();
    }
    return result;
}

std::unordered_map<ResourceType, float> ResourceManager::GetRegenRates() const {
    std::unordered_map<ResourceType, float> result;
    for (const auto& [type, resource] : resources_) {
        result[type] = resource.GetRegenRate();
    }
    return result;
}

}
