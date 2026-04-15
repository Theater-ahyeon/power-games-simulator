#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <memory>
#include <typeindex>
#include <any>

namespace PowerGame {

class EventBus {
public:
    static EventBus& Instance() {
        static EventBus instance;
        return instance;
    }

    template<typename EventType>
    void Subscribe(const std::string& channel, std::function<void(const EventType&)> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto key = channel + typeid(EventType).name();
        subscribers_[key].push_back([handler](const std::any& event) {
            handler(std::any_cast<const EventType&>(event));
        });
    }

    void Subscribe(const std::string& channel, std::function<void()> handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        voidSubscribers_[channel].push_back(handler);
    }

    template<typename EventType>
    void Publish(const std::string& channel, const EventType& event) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto key = channel + typeid(EventType).name();
        auto it = subscribers_.find(key);
        if (it != subscribers_.end()) {
            for (auto& handler : it->second) {
                handler(event);
            }
        }
    }

    void Publish(const std::string& channel) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = voidSubscribers_.find(channel);
        if (it != voidSubscribers_.end()) {
            for (auto& handler : it->second) {
                handler();
            }
        }
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_.clear();
        voidSubscribers_.clear();
    }

private:
    EventBus() = default;
    std::mutex mutex_;
    std::unordered_map<std::string, std::vector<std::function<void(const std::any&)>>> subscribers_;
    std::unordered_map<std::string, std::vector<std::function<void()>>> voidSubscribers_;
};

namespace Events {
    constexpr const char* TurnAdvanced = "TurnAdvanced";
    constexpr const char* ResourceChanged = "ResourceChanged";
    constexpr const char* FactionCreated = "FactionCreated";
    constexpr const char* FactionDestroyed = "FactionDestroyed";
    constexpr const char* AllianceFormed = "AllianceFormed";
    constexpr const char* AllianceBroken = "AllianceBroken";
    constexpr const char* BetrayalOccurred = "BetrayalOccurred";
    constexpr const char* StoryEventTriggered = "StoryEventTriggered";
    constexpr const char* StoryChoiceMade = "StoryChoiceMade";
    constexpr const char* DecisionMade = "DecisionMade";
    constexpr const char* CharacterRecruited = "CharacterRecruited";
    constexpr const char* CharacterDefected = "CharacterDefected";
    constexpr const char* IntelligenceGathered = "IntelligenceGathered";
    constexpr const char* SpyDiscovered = "SpyDiscovered";
    constexpr const char* EndingTriggered = "EndingTriggered";
    constexpr const char* LoyaltyChanged = "LoyaltyChanged";
    constexpr const char* ReputationChanged = "ReputationChanged";
    constexpr const char* ThreatDetected = "ThreatDetected";
}

struct ResourceChangeEvent {
    enum Type : int {
        PoliticalCapital = 0, Influence, Information, Wealth,
        Military, Reputation, Legitimacy, PublicSupport
    };
    Type type;
    float oldValue;
    float newValue;
    std::string reason;
};

struct FactionEvent {
    std::string factionId;
    std::string details;
};

struct AllianceEvent {
    std::string allianceId;
    std::string factionA;
    std::string factionB;
};

struct BetrayalEvent {
    std::string betrayerId;
    std::string victimId;
    float severity;
    std::string reason;
};

struct StoryEvent {
    std::string nodeId;
    std::string choiceId;
    int turn;
};

struct CharacterEvent {
    std::string characterId;
    std::string action;
    std::string details;
};

struct IntelligenceEvent {
    std::string sourceId;
    std::string targetId;
    std::string intelType;
    float value;
};

struct EndingEvent {
    std::string endingId;
    std::string endingName;
    std::vector<std::string> triggeredBy;
};

}
