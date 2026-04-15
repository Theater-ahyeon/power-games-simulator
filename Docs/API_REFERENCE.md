# Power Game API 参考文档

## 概述

本文档提供了Power Game项目的完整API参考，涵盖所有核心系统、GUI组件和游戏逻辑接口。

## 核心系统 (Core)

### GameManager

游戏主管理器，负责游戏初始化、更新循环和系统协调。

```cpp
class GameManager {
public:
    static GameManager& Instance();
    
    void Initialize();
    void Update(float deltaTime);
    void Shutdown();
    
    int GetCurrentTurn() const;
    void SetCurrentTurn(int turn);
    
    float GetResource(const std::string& name) const;
    void ModifyResource(const std::string& name, float delta);
    
    GameState GetState() const;
    void SetState(GameState state);
    
    void ProcessTurnEnd();
};
```

### EventBus

事件总线系统，实现发布-订阅模式的事件通信。

```cpp
class EventBus {
public:
    static EventBus& Instance();
    
    template<typename T>
    void Subscribe(const std::string& eventType, std::function<void(const T&)> handler);
    
    template<typename T>
    void Publish(const std::string& eventType, const T& eventData);
    
    void Unsubscribe(const std::string& eventType, int subscriberId);
};
```

## 资源系统 (Resources)

### ResourceManager

管理游戏中的8种核心资源。

#### 资源类型

| 资源ID | 名称 | 描述 |
|--------|------|------|
| `PoliticalCapital` | 政治资本 | 核心影响力指标 |
| `Influence` | 势力影响力 | 派系话语权 |
| `Information` | 情报信息 | 情报网络资源 |
| `Wealth` | 财富 | 经济实力 |
| `Military` | 军事力量 | 武装力量控制 |
| `Reputation` | 声望声誉 | 公众形象 |
| `Legitimacy` | 合法性 | 执政正当性 |
| `PublicSupport` | 民众支持 | 基层民意 |

#### API方法

```cpp
class ResourceManager {
public:
    static ResourceManager& Instance();
    
    void Initialize();
    
    float GetResource(const std::string& resourceId) const;
    void SetResource(const std::string& resourceId, float value);
    void ModifyResource(const std::string& resourceId, float delta);
    
    bool ConvertResource(const std::string& from, const std::string& to, float amount);
    
    float GetEfficiency(const std::string& resourceId) const;
    
    void ProcessTurn();
    
    std::vector<std::string> GetAllResources() const;
    float GetTotalPowerIndex() const;
};
```

## 派系系统 (Factions)

### FactionManager

管理5大政治派系及其关系。

#### 派系类型

| 派系ID | 名称 | 政治立场 | 特点 |
|--------|------|----------|------|
| `conservatives` | 保守联盟 | 传统主义 | 高财富、高军事 |
| `liberals` | 自由联盟 | 进步主义 | 高声望、高诚信 |
| `neutrals` | 中间集团 | 实用主义 | 资源均衡 |
| `radicals` | 激进阵线 | 激进主义 | 高野心、高谋略 |
| `opportunists` | 机会主义圈 | 投机主义 | 高情报、高智慧 |

#### API方法

```cpp
class FactionManager {
public:
    static FactionManager& Instance();
    
    void Initialize();
    
    int GetFactionCount() const;
    Faction* GetFaction(int id);
    Faction* GetFactionByName(const std::string& name);
    
    float GetFactionTrust(int factionId) const;
    float GetFactionPower(int factionId) const;
    float GetFactionLoyalty(int factionId) const;
    
    void ModifyFactionTrust(int factionId, float delta);
    void ModifyFactionPower(int factionId, float delta);
    
    bool FormAlliance(int factionId);
    bool BreakAlliance(int factionId);
    bool NegotiateTreaty(int factionId, TreatyType type);
    
    std::vector<int> GetAlliedFactions() const;
    std::vector<int> GetHostileFactions() const;
};
```

## 情报系统 (Intelligence)

### IntelligenceManager

管理间谍网络和情报收集。

```cpp
class IntelligenceManager {
public:
    static IntelligenceManager& Instance();
    
    void Initialize();
    
    bool DeployAgent(int factionId, const std::string& codename);
    bool ExtractAgent(const std::string& codename);
    
    float GetIntelligenceQuality(int factionId) const;
    float GetDetectionRisk(int factionId) const;
    
    void ProcessIntelligenceNetwork();
    
    IntelligenceReport* GatherReport(int factionId);
    bool AnalyzeIntelligence(const IntelligenceReport& report);
    
    void InitiateCounterIntelligence();
    void SecuritySweep();
    
    int GetActiveAgents() const;
    int GetAgentCapacity() const;
};
```

## 角色系统 (Characters)

### CharacterManager

管理22位独特NPC角色。

```cpp
class CharacterManager {
public:
    static CharacterManager& Instance();
    
    void Initialize();
    
    Character* GetCharacter(int id);
    Character* GetCharacterByName(const std::string& name);
    
    std::vector<Character*> GetCharactersByFaction(int factionId);
    std::vector<Character*> GetCharactersByStance(CharacterStance stance);
    
    bool RecruitCharacter(int characterId);
    bool DismissCharacter(int characterId);
    
    float GetCharacterLoyalty(int characterId) const;
    void ModifyCharacterLoyalty(int characterId, float delta);
    
    std::vector<Character*> GetAvailableRecruits() const;
    
    void UpdateCharacterAI(float deltaTime);
};
```

### 角色属性

| 属性 | 描述 | 范围 |
|------|------|------|
| `Scheming` | 谋略 | 0-100 |
| `Charisma` | 魅力 | 0-100 |
| `Intelligence` | 智慧 | 0-100 |
| `Wealth` | 财富 | 0-100 |
| `Military` | 军事 | 0-100 |
| `Reputation` | 声望 | 0-100 |
| `Integrity` | 诚信 | 0-100 |
| `Ambition` | 野心 | 0-100 |

### 隐藏动机

```cpp
struct HiddenMotive {
    std::string personalGoal;       // 个人目标
    std::string secretRelationship; // 秘密关系
    float betrayalTendency;          // 背叛倾向 (0-100)
    std::string triggerCondition;    // 触发条件
    std::string hiddenAgenda;        // 隐藏议程
};
```

## 剧情系统 (Story)

### StoryManager

管理8个关键剧情节点和分支叙事。

```cpp
class StoryManager {
public:
    static StoryManager& Instance();
    
    void Initialize();
    
    StoryNode* GetCurrentNode();
    void SetCurrentNode(int nodeId);
    
    std::vector<StoryChoice> GetChoices() const;
    void MakeChoice(int choiceId);
    
    bool TriggerEvent(const std::string& eventId);
    void ProcessRandomEvent();
    
    std::string GetCurrentNarrative() const;
    
    std::vector<StoryNode*> GetUnlockedNodes() const;
    int GetStoryProgress() const;
};
```

### 剧情节点

| 节点ID | 名称 | 分支数 |
|--------|------|--------|
| 0 | 权力真空 | 3 |
| 1 | 派系角力 | 4 |
| 2 | 情报战争 | 3 |
| 3 | 经济危机 | 4 |
| 4 | 外国干预 | 3 |
| 5 | 宪法危机 | 5 |
| 6 | 关键抉择 | 4 |
| 7 | 最终博弈 | 5 |

## 策略系统 (Strategy)

### StrategyManager

管理决策树和多层博弈。

```cpp
class StrategyManager {
public:
    static StrategyManager& Instance();
    
    void Initialize();
    
    DecisionTree* GetCurrentDecision();
    
    std::vector<StrategyOption> AnalyzeOptions();
    StrategyOption SelectOption(int optionId);
    
    float CalculateSuccessProbability(int optionId);
    std::vector<Effect> CalculateEffects(int optionId);
    
    void ApplyDecision(int optionId);
    void ProcessDelayedEffects();
    
    DecisionHistory* GetHistory();
};
```

## 结局系统 (Ending)

### EndingManager

管理8种游戏结局评估。

```cpp
class EndingManager {
public:
    static EndingManager& Instance();
    
    void Initialize();
    
    std::string DetermineEnding();
    std::string GetEndingName(const std::string& endingId) const;
    std::string GetEndingDescription(const std::string& endingId) const;
    
    float GetEndingProbability(const std::string& endingId) const;
    
    std::vector<std::string> GetAvailableEndings() const;
    
    bool UnlockEnding(const std::string& endingId);
    std::vector<std::string> GetUnlockedEndings() const;
};
```

### 结局类型

| 结局ID | 名称 | 条件 |
|--------|------|------|
| `dictatorship` | 独裁统治 | 政治资本>90, 军事>80, 合法性<30 |
| `democratic_reform` | 民主改革 | 声望>85, 民众支持>75, 合法性>70 |
| `power_balance` | 权力平衡 | 无派系>60%权力, 稳定性>65, 全派系友好 |
| `exile` | 流亡海外 | 合法性<15, 全派系敌对 |
| `shadow_control` | 幕后操控 | 情报>90, 公众形象<30, 深度潜伏 |
| `system_collapse` | 系统崩溃 | 稳定性<25, 全派系敌对, 经济危机活跃 |
| `foreign_intervention` | 外国干预 | 外国访问>60, 国家主权<50 |
| `personal_ambition` | 个人野心 | 野心>90, 诚信<30, 为个人利益牺牲盟友 |

## GUI系统

### GUIRenderer

SDL2图形渲染器。

```cpp
class GUIRenderer {
public:
    bool Initialize(int width, int height);
    void Shutdown();
    
    void Clear();
    void Present();
    
    void DrawRect(int x, int y, int w, int h, bool filled);
    void DrawLine(int x1, int y1, int x2, int y2);
    void DrawText(const std::string& text, int x, int y, TTF_Font* font, const Color& color);
    void DrawTexture(SDL_Texture* texture, int x, int y, int w, int h);
    
    void DrawProgressBar(int x, int y, int w, int h, float progress, const Color& fillColor, const Color& bgColor);
    void DrawPanel(int x, int y, int w, int h, const Color& bgColor);
    
    SDL_Renderer* GetSDLRenderer() const;
    SDL_Window* GetWindow() const;
    
    int GetWidth() const;
    int GetHeight() const;
};
```

### GUIComponents

GUI组件库。

#### 可用组件

| 组件 | 描述 |
|------|------|
| `GUIButton` | 交互按钮 |
| `GUILabel` | 文本标签 |
| `GUIProgressBar` | 进度条 |
| `GUIPanel` | 面板容器 |
| `GUIImage` | 图像显示 |
| `GUIListBox` | 列表框 |

#### 基类接口

```cpp
class GUIComponent {
public:
    virtual void Render(GUIRenderer* renderer) = 0;
    virtual bool HandleEvent(const SDL_Event& event) = 0;
    
    void SetPosition(int x, int y);
    void SetSize(int w, int h);
    void SetVisible(bool visible);
    void SetEnabled(bool enabled);
    
    bool IsVisible() const;
    bool IsEnabled() const;
    bool ContainsPoint(int px, int py) const;
};
```

## 存档系统

### SaveManager

游戏数据持久化。

```cpp
class SaveManager {
public:
    static SaveManager& Instance();
    
    bool SaveGame(const std::string& savePath);
    bool LoadGame(const std::string& savePath);
    
    std::vector<SaveInfo> GetAllSaves() const;
    bool DeleteSave(const std::string& savePath);
    
    bool AutoSave();
    bool QuickSave();
    bool QuickLoad();
    
    std::string GetAutoSavePath() const;
    std::string GetQuickSavePath() const;
};
```

## AI系统

### AIDecisionEngine

NPC角色AI决策。

```cpp
class AIDecisionEngine {
public:
    static AIDecisionEngine& Instance();
    
    void Initialize();
    
    void UpdateCharacterAI(Character* character, float deltaTime);
    
    Action* DecideAction(Character* character);
    std::vector<Action*> GeneratePossibleActions(Character* character);
    
    float EvaluateActionUtility(Action* action, Character* character);
    
    void ExecuteAction(Action* action);
};
```

## 音频系统

### AudioManager

SDL2_mixer音频管理。

```cpp
class AudioManager {
public:
    static AudioManager& Instance();
    
    bool Initialize();
    void Shutdown();
    
    void PlayMusic(MusicTrack track, int loops = -1);
    void StopMusic();
    void PauseMusic();
    void ResumeMusic();
    void SetMusicVolume(float volume);
    
    void PlaySoundEffect(SoundEffect effect);
    void SetSoundEffectVolume(float volume);
    
    bool LoadSoundEffect(SoundEffect effect, const std::string& filePath);
    bool LoadMusic(MusicTrack track, const std::string& filePath);
};
```

## 成就系统

### AchievementSystem

玩家成就追踪。

```cpp
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
    
    int GetTotalCount() const;
    int GetUnlockedCount() const;
    float GetCompletionPercentage() const;
};
```

## 使用示例

### 创建新游戏

```cpp
#include "Core/GameManager.h"
#include "Resources/ResourceManager.h"
#include "Faction/FactionManager.h"

int main() {
    auto& gameMgr = GameManager::Instance();
    auto& resourceMgr = ResourceManager::Instance();
    auto& factionMgr = FactionManager::Instance();
    
    gameMgr.Initialize();
    
    // 设置初始资源
    resourceMgr.SetResource("PoliticalCapital", 50);
    resourceMgr.SetResource("Influence", 50);
    resourceMgr.SetResource("Information", 30);
    resourceMgr.SetResource("Wealth", 40);
    resourceMgr.SetResource("Military", 30);
    resourceMgr.SetResource("Reputation", 50);
    resourceMgr.SetResource("Legitimacy", 60);
    resourceMgr.SetResource("PublicSupport", 50);
    
    // 游戏主循环
    while (gameMgr.GetState() != GameState::GAME_OVER) {
        gameMgr.Update(0.016f);
    }
    
    gameMgr.Shutdown();
    return 0;
}
```

### 资源转换

```cpp
auto& resourceMgr = ResourceManager::Instance();

// 将财富转换为影响力
bool success = resourceMgr.ConvertResource("Wealth", "Influence", 20);

if (success) {
    std::cout << "转换成功！" << std::endl;
} else {
    std::cout << "资源不足或转换条件不满足" << std::endl;
}
```

### 派系交互

```cpp
auto& factionMgr = FactionManager::Instance();

// 获取保守联盟
auto* conservative = factionMgr.GetFactionByName("conservatives");

// 提高信任度
factionMgr.ModifyFactionTrust(conservative->id, 10);

// 建立联盟
factionMgr.FormAlliance(conservative->id);
```

### GUI按钮创建

```cpp
auto renderer = std::make_unique<GUIRenderer>();
renderer->Initialize(1280, 720);

auto button = std::make_unique<GUIButton>(
    100, 100, 200, 50, "开始游戏"
);

button->SetClickHandler([]() {
    std::cout << "游戏开始！" << std::endl;
});

while (running) {
    button->Render(renderer.get());
    renderer->Present();
}
```

## 最佳实践

### 初始化顺序

```cpp
// 正确的初始化顺序
GameManager::Instance().Initialize();
ResourceManager::Instance().Initialize();
FactionManager::Instance().Initialize();
IntelligenceManager::Instance().Initialize();
CharacterManager::Instance().Initialize();
StoryManager::Instance().Initialize();
AchievementSystem::Instance().Initialize();
```

### 事件处理

```cpp
// 订阅资源变化事件
EventBus::Instance().Subscribe<ResourceChangedEvent>("resource_changed", 
    [](const ResourceChangedEvent& event) {
        std::cout << event.resourceId << " 变为 " << event.newValue << std::endl;
    }
);

// 发布事件
ResourceChangedEvent event = {"PoliticalCapital", 75.0f};
EventBus::Instance().Publish("resource_changed", event);
```

### 存档保存

```cpp
auto& saveMgr = SaveManager::Instance();

// 自动保存
saveMgr.AutoSave();

// 手动保存
saveMgr.SaveGame("saves/manual_save_" + currentDate + ".json");
```

## 许可

本文档遵循MIT许可证。
