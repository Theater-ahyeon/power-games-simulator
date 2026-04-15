#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include "../Core/GameSystem.h"

namespace PowerGame {

struct SaveData {
    std::string saveId;
    std::string saveName;
    int turn;
    std::string timestamp;

    std::unordered_map<std::string, float> resources;
    std::unordered_map<std::string, std::string> factionStates;
    std::unordered_map<std::string, std::string> characterStates;
    std::vector<std::string> storyDecisions;
    std::vector<std::string> activeTreaties;
    std::unordered_map<std::string, std::string> spyStates;
    std::unordered_map<std::string, float> trustMatrix;
    std::vector<std::string> betrayalHistory;
    std::string gameState;
};

class SaveManager {
public:
    static SaveManager& Instance() {
        static SaveManager instance;
        return instance;
    }

    bool SaveGame(const std::string& saveName);
    bool LoadGame(const std::string& saveId);
    bool DeleteSave(const std::string& saveId);
    std::vector<SaveData> GetSaveList() const;
    bool HasSave(const std::string& saveId) const;

    void SetSaveDirectory(const std::string& dir) { saveDirectory_ = dir; }
    const std::string& GetSaveDirectory() const { return saveDirectory_; }

private:
    SaveManager() = default;
    std::string saveDirectory_ = "saves/";
    std::unordered_map<std::string, SaveData> saves_;

    SaveData CaptureGameState() const;
    bool RestoreGameState(const SaveData& data);
    std::string SerializeSave(const SaveData& data) const;
    std::optional<SaveData> DeserializeSave(const std::string& serialized) const;
    std::string GenerateSaveId() const;
};

class DataSerializer {
public:
    static std::string SerializeResources(const std::unordered_map<std::string, float>& resources);
    static std::unordered_map<std::string, float> DeserializeResources(const std::string& data);

    static std::string SerializeStringMap(const std::unordered_map<std::string, std::string>& map);
    static std::unordered_map<std::string, std::string> DeserializeStringMap(const std::string& data);

    static std::string SerializeStringList(const std::vector<std::string>& list);
    static std::vector<std::string> DeserializeStringList(const std::string& data);

    static std::string SerializeFloatMap(const std::unordered_map<std::string, float>& map);
    static std::unordered_map<std::string, float> DeserializeFloatMap(const std::string& data);
};

}
