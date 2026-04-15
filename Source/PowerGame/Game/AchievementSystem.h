#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace PowerGame {

struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    std::string icon;
    int difficulty; // 1-5
    bool unlocked = false;
    std::chrono::system_clock::time_point unlockTime;
    std::string unlockDescription;
    
    std::function<bool()> checkCondition;
};

class AchievementSystem {
public:
    static AchievementSystem& Instance();
    
    void Initialize();
    void Shutdown();
    
    void RegisterAchievement(const Achievement& achievement);
    void CheckAllAchievements();
    void UnlockAchievement(const std::string& achievementId);
    
    bool IsAchievementUnlocked(const std::string& achievementId) const;
    std::vector<Achievement> GetUnlockedAchievements() const;
    std::vector<Achievement> GetLockedAchievements() const;
    
    int GetTotalCount() const { return static_cast<int>(achievements_.size()); }
    int GetUnlockedCount() const;
    float GetCompletionPercentage() const;
    
    void NotifyEvent(const std::string& eventType, const std::string& data);

private:
    AchievementSystem() = default;
    
    std::unordered_map<std::string, Achievement> achievements_;
    std::vector<std::string> recentUnlocks_;
    
    void CheckPoliticalAchievements();
    void CheckResourceAchievements();
    void CheckFactionAchievements();
    void CheckTimeAchievements();
};

class AchievementNotification {
public:
    static void Show(const std::string& achievementName, const std::string& description);
    static void Update();
    static void Render();
    static bool IsActive();

private:
    static bool active_;
    static std::string currentAchievement_;
    static std::string currentDescription_;
    static float displayTime_;
    static float maxDisplayTime_;
};

}
