#include "SaveManager.h"
#include "../Core/GameManager.h"
#include "../Resources/ResourceManager.h"
#include "../Faction/FactionManager.h"
#include "../Character/CharacterManager.h"
#include "../Intelligence/IntelligenceManager.h"
#include "../Alliance/AllianceManager.h"
#include "../Story/StoryManager.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace PowerGame {

bool SaveManager::SaveGame(const std::string& saveName) {
    SaveData data = CaptureGameState();
    data.saveId = GenerateSaveId();
    data.saveName = saveName;

    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    data.timestamp = ss.str();

    saves_[data.saveId] = data;

    std::string serialized = SerializeSave(data);
    std::string filepath = saveDirectory_ + data.saveId + ".sav";
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) return false;
    file.write(serialized.data(), serialized.size());
    file.close();

    return true;
}

bool SaveManager::LoadGame(const std::string& saveId) {
    auto it = saves_.find(saveId);
    if (it == saves_.end()) return false;
    return RestoreGameState(it->second);
}

bool SaveManager::DeleteSave(const std::string& saveId) {
    auto it = saves_.find(saveId);
    if (it == saves_.end()) return false;
    saves_.erase(it);
    return true;
}

std::vector<SaveData> SaveManager::GetSaveList() const {
    std::vector<SaveData> result;
    for (const auto& [id, data] : saves_) {
        result.push_back(data);
    }
    std::sort(result.begin(), result.end(),
        [](const SaveData& a, const SaveData& b) { return a.timestamp > b.timestamp; });
    return result;
}

bool SaveManager::HasSave(const std::string& saveId) const {
    return saves_.find(saveId) != saves_.end();
}

SaveData SaveManager::CaptureGameState() const {
    SaveData data;
    auto& gameMgr = GameManager::Instance();
    data.turn = gameMgr.GetCurrentTurn();
    data.gameState = std::to_string(static_cast<int>(gameMgr.GetCurrentState()));

    auto* resourceMgr = GameSystemManager::Instance().GetSystem<ResourceManager>();
    if (resourceMgr) {
        auto allRes = resourceMgr->GetAllResources();
        for (const auto& [type, value] : allRes) {
            data.resources[ResourceTypeToString(type)] = value;
        }
    }

    auto* factionMgr = GameSystemManager::Instance().GetSystem<FactionManager>();
    if (factionMgr) {
        for (const auto& [id, faction] : factionMgr->GetAllFactions()) {
            data.factionStates[id] = std::to_string(faction->GetPower());
        }
    }

    auto* charMgr = GameSystemManager::Instance().GetSystem<CharacterManager>();
    if (charMgr) {
        for (const auto& [id, character] : charMgr->GetAllCharacters()) {
            data.characterStates[id] = std::to_string(static_cast<int>(character->GetStance()));
        }
    }

    auto* storyMgr = GameSystemManager::Instance().GetSystem<StoryManager>();
    if (storyMgr) {
        data.storyDecisions = storyMgr->GetDecisionHistory();
    }

    return data;
}

bool SaveManager::RestoreGameState(const SaveData& data) {
    auto& gameMgr = GameManager::Instance();
    return true;
}

std::string SaveManager::SerializeSave(const SaveData& data) const {
    std::stringstream ss;
    ss << "SAVE_V1\n";
    ss << "id:" << data.saveId << "\n";
    ss << "name:" << data.saveName << "\n";
    ss << "turn:" << data.turn << "\n";
    ss << "timestamp:" << data.timestamp << "\n";
    ss << "state:" << data.gameState << "\n";

    ss << "resources:\n";
    for (const auto& [key, value] : data.resources) {
        ss << "  " << key << ":" << value << "\n";
    }

    ss << "decisions:\n";
    for (const auto& decision : data.storyDecisions) {
        ss << "  " << decision << "\n";
    }

    return ss.str();
}

std::optional<SaveData> SaveManager::DeserializeSave(const std::string& serialized) const {
    SaveData data;
    std::istringstream ss(serialized);
    std::string line;

    if (!std::getline(ss, line) || line != "SAVE_V1") return std::nullopt;

    while (std::getline(ss, line)) {
        if (line == "resources:") {
            while (std::getline(ss, line) && line.substr(0, 2) == "  ") {
                size_t colon = line.find(':', 2);
                if (colon != std::string::npos) {
                    std::string key = line.substr(2, colon - 2);
                    float value = std::stof(line.substr(colon + 1));
                    data.resources[key] = value;
                }
            }
        }
    }

    return data;
}

std::string SaveManager::GenerateSaveId() const {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto count = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return "save_" + std::to_string(count);
}

std::string DataSerializer::SerializeResources(const std::unordered_map<std::string, float>& resources) {
    std::stringstream ss;
    for (const auto& [key, value] : resources) {
        ss << key << "=" << value << ";";
    }
    return ss.str();
}

std::unordered_map<std::string, float> DataSerializer::DeserializeResources(const std::string& data) {
    std::unordered_map<std::string, float> result;
    std::istringstream ss(data);
    std::string token;
    while (std::getline(ss, token, ';')) {
        size_t eq = token.find('=');
        if (eq != std::string::npos) {
            result[token.substr(0, eq)] = std::stof(token.substr(eq + 1));
        }
    }
    return result;
}

std::string DataSerializer::SerializeStringMap(const std::unordered_map<std::string, std::string>& map) {
    std::stringstream ss;
    for (const auto& [key, value] : map) {
        ss << key << "=" << value << ";";
    }
    return ss.str();
}

std::unordered_map<std::string, std::string> DataSerializer::DeserializeStringMap(const std::string& data) {
    std::unordered_map<std::string, std::string> result;
    std::istringstream ss(data);
    std::string token;
    while (std::getline(ss, token, ';')) {
        size_t eq = token.find('=');
        if (eq != std::string::npos) {
            result[token.substr(0, eq)] = token.substr(eq + 1);
        }
    }
    return result;
}

std::string DataSerializer::SerializeStringList(const std::vector<std::string>& list) {
    std::stringstream ss;
    for (const auto& item : list) {
        ss << item << ";";
    }
    return ss.str();
}

std::vector<std::string> DataSerializer::DeserializeStringList(const std::string& data) {
    std::vector<std::string> result;
    std::istringstream ss(data);
    std::string token;
    while (std::getline(ss, token, ';')) {
        if (!token.empty()) result.push_back(token);
    }
    return result;
}

std::string DataSerializer::SerializeFloatMap(const std::unordered_map<std::string, float>& map) {
    return SerializeResources(map);
}

std::unordered_map<std::string, float> DataSerializer::DeserializeFloatMap(const std::string& data) {
    return DeserializeResources(data);
}

}
