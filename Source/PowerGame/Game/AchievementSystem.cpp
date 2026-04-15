#include "AchievementSystem.h"
#include "Core/GameManager.h"
#include "Resources/ResourceManager.h"
#include "Faction/FactionManager.h"

namespace PowerGame {

bool AchievementNotification::active_ = false;
std::string AchievementNotification::currentAchievement_;
std::string AchievementNotification::currentDescription_;
float AchievementNotification::displayTime_ = 0.0f;
float AchievementNotification::maxDisplayTime_ = 5.0f;

AchievementSystem& AchievementSystem::Instance() {
    static AchievementSystem instance;
    return instance;
}

void AchievementSystem::Initialize() {
    achievements_.clear();
    
    achievements_["first_steps"] = {
        "first_steps",
        "初出茅庐",
        "完成第一回合",
        "trophy_basic",
        1,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            auto& gameMgr = GameManager::Instance();
            return gameMgr.GetCurrentTurn() >= 1;
        }
    };
    
    achievements_["survivor"] = {
        "survivor",
        "幸存者",
        "在危机中存活10回合",
        "trophy_silver",
        2,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            auto& gameMgr = GameManager::Instance();
            return gameMgr.GetCurrentTurn() >= 10;
        }
    };
    
    achievements_["master_strategist"] = {
        "master_strategist",
        "战略大师",
        "连续做出3个成功决策",
        "trophy_gold",
        3,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            auto& gameMgr = GameManager::Instance();
            return gameMgr.GetSuccessStreak() >= 3;
        }
    };
    
    achievements_["network_builder"] = {
        "network_builder",
        "关系网编织者",
        "与所有5个派系建立友好关系",
        "network_icon",
        3,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            auto& factionMgr = FactionManager::Instance();
            int friendlyCount = 0;
            for (int i = 0; i < 5; ++i) {
                if (factionMgr.GetFactionTrust(i) >= 70) {
                    friendlyCount++;
                }
            }
            return friendlyCount >= 5;
        }
    };
    
    achievements_["intelligence_master"] = {
        "intelligence_master",
        "情报大师",
        "情报收集达到100点",
        "intel_icon",
        2,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            auto& resourceMgr = ResourceManager::Instance();
            return resourceMgr.GetResource("Information") >= 100;
        }
    };
    
    achievements_["wealthy_politician"] = {
        "wealthy_politician",
        "富甲一方",
        "财富积累达到150点",
        "wealth_icon",
        2,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            auto& resourceMgr = ResourceManager::Instance();
            return resourceMgr.GetResource("Wealth") >= 150;
        }
    };
    
    achievements_["popular_leader"] = {
        "popular_leader",
        "民心所向",
        "民众支持率达到90%",
        "people_icon",
        3,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            auto& resourceMgr = ResourceManager::Instance();
            return resourceMgr.GetResource("PublicSupport") >= 90;
        }
    };
    
    achievements_["betrayer"] = {
        "betrayer",
        "背叛者",
        "背叛一个盟友",
        "skull_icon",
        4,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            return GameManager::Instance().GetBetrayalCount() >= 1;
        }
    };
    
    achievements_["diplomat"] = {
        "diplomat",
        "外交大师",
        "成功谈判5个条约",
        "treaty_icon",
        3,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            return GameManager::Instance().GetTreatyCount() >= 5;
        }
    };
    
    achievements_["shadow_master"] = {
        "shadow_master",
        "影子大师",
        "在公众支持低于30%的情况下仍然掌权",
        "shadow_icon",
        5,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            auto& resourceMgr = ResourceManager::Instance();
            return resourceMgr.GetResource("PublicSupport") < 30 && 
                   resourceMgr.GetResource("PoliticalCapital") >= 70;
        }
    };
    
    achievements_["reformer"] = {
        "reformer",
        "改革者",
        "通过宪法改革",
        "reform_icon",
        4,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            return GameManager::Instance().HasReformPassed();
        }
    };
    
    achievements_["dictator"] = {
        "dictator",
        "独裁者",
        "获得独裁结局",
        "crown_icon",
        5,
        false,
        std::chrono::system_clock::now(),
        "",
        []() {
            return GameManager::Instance().GetEndingId() == "dictatorship";
        }
    };
}

void AchievementSystem::Shutdown() {
    achievements_.clear();
}

void AchievementSystem::RegisterAchievement(const Achievement& achievement) {
    achievements_[achievement.id] = achievement;
}

void AchievementSystem::CheckAllAchievements() {
    for (auto& pair : achievements_) {
        if (!pair.second.unlocked && pair.second.checkCondition()) {
            UnlockAchievement(pair.second.id);
        }
    }
}

void AchievementSystem::UnlockAchievement(const std::string& achievementId) {
    auto it = achievements_.find(achievementId);
    if (it != achievements_.end() && !it->second.unlocked) {
        it->second.unlocked = true;
        it->second.unlockTime = std::chrono::system_clock::now();
        it->second.unlockDescription = "解锁于回合 " + 
            std::to_string(GameManager::Instance().GetCurrentTurn());
        
        AchievementNotification::Show(it->second.name, it->second.description);
        
        recentUnlocks_.push_back(achievementId);
        if (recentUnlocks_.size() > 10) {
            recentUnlocks_.erase(recentUnlocks_.begin());
        }
    }
}

bool AchievementSystem::IsAchievementUnlocked(const std::string& achievementId) const {
    auto it = achievements_.find(achievementId);
    return it != achievements_.end() && it->second.unlocked;
}

std::vector<Achievement> AchievementSystem::GetUnlockedAchievements() const {
    std::vector<Achievement> result;
    for (const auto& pair : achievements_) {
        if (pair.second.unlocked) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Achievement> AchievementSystem::GetLockedAchievements() const {
    std::vector<Achievement> result;
    for (const auto& pair : achievements_) {
        if (!pair.second.unlocked) {
            result.push_back(pair.second);
        }
    }
    return result;
}

int AchievementSystem::GetUnlockedCount() const {
    int count = 0;
    for (const auto& pair : achievements_) {
        if (pair.second.unlocked) {
            count++;
        }
    }
    return count;
}

float AchievementSystem::GetCompletionPercentage() const {
    if (achievements_.empty()) return 0.0f;
    return static_cast<float>(GetUnlockedCount()) / static_cast<float>(GetTotalCount()) * 100.0f;
}

void AchievementSystem::NotifyEvent(const std::string& eventType, const std::string& data) {
    CheckAllAchievements();
}

void AchievementNotification::Show(const std::string& achievementName, const std::string& description) {
    currentAchievement_ = achievementName;
    currentDescription_ = description;
    displayTime_ = 0.0f;
    active_ = true;
}

void AchievementNotification::Update() {
    if (active_) {
        displayTime_ += 0.016f;
        if (displayTime_ >= maxDisplayTime_) {
            active_ = false;
        }
    }
}

void AchievementNotification::Render() {
    // Rendering handled by GUI system
}

bool AchievementNotification::IsActive() {
    return active_;
}

}
