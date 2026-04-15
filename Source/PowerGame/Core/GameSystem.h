#pragma once
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <typeindex>

namespace PowerGame {

class IGameSystem {
public:
    virtual ~IGameSystem() = default;
    virtual void Initialize() = 0;
    virtual void Reset() = 0;
    virtual void OnTurnUpdate(int turn) = 0;
    virtual void OnFrameUpdate(float deltaTime) = 0;
    virtual std::string GetSystemName() const = 0;
};

class GameSystemManager {
public:
    static GameSystemManager& Instance() {
        static GameSystemManager instance;
        return instance;
    }

    template<typename T, typename... Args>
    T* RegisterSystem(Args&&... args) {
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        auto* ptr = system.get();
        systems_.push_back(std::move(system));
        systemMap_[std::type_index(typeid(T))] = ptr;
        return ptr;
    }

    template<typename T>
    T* GetSystem() {
        auto it = systemMap_.find(std::type_index(typeid(T)));
        if (it != systemMap_.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }

    void InitializeAll() {
        for (auto& system : systems_) {
            system->Initialize();
        }
    }

    void ResetAll() {
        for (auto& system : systems_) {
            system->Reset();
        }
    }

    void UpdateAllTurn(int turn) {
        for (auto& system : systems_) {
            system->OnTurnUpdate(turn);
        }
    }

    void UpdateAllFrame(float deltaTime) {
        for (auto& system : systems_) {
            system->OnFrameUpdate(deltaTime);
        }
    }

private:
    GameSystemManager() = default;
    std::vector<std::unique_ptr<IGameSystem>> systems_;
    std::unordered_map<std::type_index, IGameSystem*> systemMap_;
};

}
