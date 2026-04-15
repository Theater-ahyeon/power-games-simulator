#pragma once
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace PowerGame {

class GameApplication {
public:
    static GameApplication& Instance() {
        static GameApplication instance;
        return instance;
    }

    int Run(int argc, char* argv[]);
    void Shutdown();

    void SetTargetFPS(int fps) { targetFPS_ = fps; }
    int GetTargetFPS() const { return targetFPS_; }
    bool IsRunning() const { return running_; }

private:
    GameApplication() = default;

    void Initialize();
    void GameLoop();
    void ProcessInput();
    void Update(float deltaTime);
    void Render();
    void Cleanup();

    bool running_ = false;
    bool initialized_ = false;
    int targetFPS_ = 60;
    float deltaTime_ = 0.0f;
};

}
