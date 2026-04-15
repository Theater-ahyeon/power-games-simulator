#pragma once
#include <SDL2/SDL.h>
#include <functional>
#include <vector>
#include <memory>
#include <cmath>

namespace PowerGame {

enum class EasingType {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Bounce,
    Elastic
};

class Animation {
public:
    Animation(float duration, EasingType easing = EasingType::EaseInOut);
    virtual ~Animation() = default;

    void Update(float deltaTime);
    bool IsComplete() const { return progress_ >= 1.0f; }
    void Reset();

    float GetProgress() const { return progress_; }
    float GetEasedProgress() const;

    using CompleteCallback = std::function<void()>;
    void SetCompleteCallback(CompleteCallback cb) { completeCallback_ = cb; }

protected:
    float duration_;
    float elapsed_ = 0.0f;
    float progress_ = 0.0f;
    EasingType easing_;
    CompleteCallback completeCallback_;
};

class FloatAnimation : public Animation {
public:
    FloatAnimation(float start, float end, float duration, EasingType easing = EasingType::EaseInOut);

    float GetCurrentValue() const;

private:
    float startValue_;
    float endValue_;
};

class Vector2Animation : public Animation {
public:
    Vector2Animation(float startX, float startY, float endX, float endY,
                     float duration, EasingType easing = EasingType::EaseInOut);

    void GetCurrentPosition(float& x, float& y) const;

private:
    float startX_, startY_;
    float endX_, endY_;
};

class ColorAnimation : public Animation {
public:
    ColorAnimation(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t a1,
                   uint8_t r2, uint8_t g2, uint8_t b2, uint8_t a2,
                   float duration, EasingType easing = EasingType::EaseInOut);

    void GetCurrentColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const;

private:
    uint8_t r1_, g1_, b1_, a1_;
    uint8_t r2_, g2_, b2_, a2_;
};

class AnimationSystem {
public:
    static AnimationSystem& Instance() {
        static AnimationSystem instance;
        return instance;
    }

    void Update(float deltaTime);

    void AddAnimation(std::shared_ptr<Animation> anim);
    void ClearCompletedAnimations();
    void ClearAllAnimations();

    std::shared_ptr<FloatAnimation> AnimateFloat(float start, float end, float duration,
                                                  EasingType easing = EasingType::EaseInOut);
    std::shared_ptr<Vector2Animation> AnimatePosition(float startX, float startY,
                                                       float endX, float endY, float duration,
                                                       EasingType easing = EasingType::EaseInOut);
    std::shared_ptr<ColorAnimation> AnimateColor(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t a1,
                                                  uint8_t r2, uint8_t g2, uint8_t b2, uint8_t a2,
                                                  float duration,
                                                  EasingType easing = EasingType::EaseInOut);

private:
    std::vector<std::shared_ptr<Animation>> animations_;
};

class VisualFeedback {
public:
    static void ShowResourceChange(GUIRenderer* renderer, int x, int y, float amount, const std::string& resourceName);
    static void ShowNotification(GUIRenderer* renderer, int x, int y, const std::string& message, const Color& color);
    static void ShowDamageNumber(GUIRenderer* renderer, int x, int y, int damage);
    static void ShowHealNumber(GUIRenderer* renderer, int x, int y, int heal);
    static void PlayButtonClickEffect(GUIRenderer* renderer, int x, int y);
    static void PlayPanelOpenEffect(GUIRenderer* renderer, int x, int y, int w, int h);
    static void PlayPanelCloseEffect(GUIRenderer* renderer, int x, int y, int w, int h);
    static void PlaySuccessEffect(GUIRenderer* renderer, int x, int y);
    static void PlayFailureEffect(GUIRenderer* renderer, int x, int y);
    static void PlayWarningEffect(GUIRenderer* renderer, int x, int y);
};

class ParticleEffect {
public:
    struct Particle {
        float x, y;
        float vx, vy;
        float life;
        float maxLife;
        uint8_t r, g, b, a;
        float size;
    };

    ParticleEffect(int x, int y, int count, const Color& color);

    void Update(float deltaTime);
    void Render(GUIRenderer* renderer);
    bool IsComplete() const;

private:
    std::vector<Particle> particles_;
    int originX_, originY_;
};

class ScreenTransition {
public:
    enum class Type {
        FadeToBlack,
        FadeFromBlack,
        SlideLeft,
        SlideRight,
        SlideUp,
        SlideDown,
        ZoomIn,
        ZoomOut,
        WipeHorizontal,
        WipeVertical
    };

    ScreenTransition(Type type, float duration);

    void Update(float deltaTime);
    void Render(GUIRenderer* renderer);
    bool IsComplete() const { return progress_ >= 1.0f; }

    void Start();
    void Reset();

private:
    Type type_;
    float duration_;
    float elapsed_ = 0.0f;
    float progress_ = 0.0f;
    bool started_ = false;
};

float Ease(EasingType type, float t);
float EaseIn(float t);
float EaseOut(float t);
float EaseInOut(float t);
float Bounce(float t);
float Elastic(float t);

}
