#include "AnimationSystem.h"
#include "GUIRenderer.h"
#include <random>
#include <cmath>

namespace PowerGame {

Animation::Animation(float duration, EasingType easing)
    : duration_(duration), easing_(easing) {}

void Animation::Update(float deltaTime) {
    if (progress_ >= 1.0f) return;

    elapsed_ += deltaTime;
    progress_ = std::min(1.0f, elapsed_ / duration_);

    if (progress_ >= 1.0f && completeCallback_) {
        completeCallback_();
    }
}

void Animation::Reset() {
    elapsed_ = 0.0f;
    progress_ = 0.0f;
}

float Animation::GetEasedProgress() const {
    return Ease(easing_, progress_);
}

FloatAnimation::FloatAnimation(float start, float end, float duration, EasingType easing)
    : Animation(duration, easing), startValue_(start), endValue_(end) {}

float FloatAnimation::GetCurrentValue() const {
    float t = GetEasedProgress();
    return startValue_ + (endValue_ - startValue_) * t;
}

Vector2Animation::Vector2Animation(float startX, float startY, float endX, float endY,
                                   float duration, EasingType easing)
    : Animation(duration, easing),
      startX_(startX), startY_(startY),
      endX_(endX), endY_(endY) {}

void Vector2Animation::GetCurrentPosition(float& x, float& y) const {
    float t = GetEasedProgress();
    x = startX_ + (endX_ - startX_) * t;
    y = startY_ + (endY_ - startY_) * t;
}

ColorAnimation::ColorAnimation(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t a1,
                               uint8_t r2, uint8_t g2, uint8_t b2, uint8_t a2,
                               float duration, EasingType easing)
    : Animation(duration, easing),
      r1_(r1), g1_(g1), b1_(b1), a1_(a1),
      r2_(r2), g2_(g2), b2_(b2), a2_(a2) {}

void ColorAnimation::GetCurrentColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const {
    float t = GetEasedProgress();
    r = static_cast<uint8_t>(r1_ + (r2_ - r1_) * t);
    g = static_cast<uint8_t>(g1_ + (g2_ - g1_) * t);
    b = static_cast<uint8_t>(b1_ + (b2_ - b1_) * t);
    a = static_cast<uint8_t>(a1_ + (a2_ - a1_) * t);
}

void AnimationSystem::Update(float deltaTime) {
    for (auto& anim : animations_) {
        if (anim && !anim->IsComplete()) {
            anim->Update(deltaTime);
        }
    }
    ClearCompletedAnimations();
}

void AnimationSystem::AddAnimation(std::shared_ptr<Animation> anim) {
    if (anim) {
        animations_.push_back(anim);
    }
}

void AnimationSystem::ClearCompletedAnimations() {
    animations_.erase(
        std::remove_if(animations_.begin(), animations_.end(),
            [](const std::shared_ptr<Animation>& anim) {
                return !anim || anim->IsComplete();
            }),
        animations_.end()
    );
}

void AnimationSystem::ClearAllAnimations() {
    animations_.clear();
}

std::shared_ptr<FloatAnimation> AnimationSystem::AnimateFloat(float start, float end, float duration, EasingType easing) {
    auto anim = std::make_shared<FloatAnimation>(start, end, duration, easing);
    AddAnimation(anim);
    return anim;
}

std::shared_ptr<Vector2Animation> AnimationSystem::AnimatePosition(float startX, float startY,
                                                                    float endX, float endY, float duration,
                                                                    EasingType easing) {
    auto anim = std::make_shared<Vector2Animation>(startX, startY, endX, endY, duration, easing);
    AddAnimation(anim);
    return anim;
}

std::shared_ptr<ColorAnimation> AnimationSystem::AnimateColor(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t a1,
                                                              uint8_t r2, uint8_t g2, uint8_t b2, uint8_t a2,
                                                              float duration, EasingType easing) {
    auto anim = std::make_shared<ColorAnimation>(r1, g1, b1, a1, r2, g2, b2, a2, duration, easing);
    AddAnimation(anim);
    return anim;
}

void VisualFeedback::ShowResourceChange(GUIRenderer* renderer, int x, int y, float amount, const std::string& resourceName) {
    // Visual feedback for resource changes would be rendered here
    // This is a placeholder for the actual implementation
}

void VisualFeedback::ShowNotification(GUIRenderer* renderer, int x, int y, const std::string& message, const Color& color) {
    // Show notification popup
}

void VisualFeedback::ShowDamageNumber(GUIRenderer* renderer, int x, int y, int damage) {
    // Show damage number floating up
}

void VisualFeedback::ShowHealNumber(GUIRenderer* renderer, int x, int y, int heal) {
    // Show heal number floating up
}

void VisualFeedback::PlayButtonClickEffect(GUIRenderer* renderer, int x, int y) {
    // Play button click visual effect
}

void VisualFeedback::PlayPanelOpenEffect(GUIRenderer* renderer, int x, int y, int w, int h) {
    // Play panel open animation
}

void VisualFeedback::PlayPanelCloseEffect(GUIRenderer* renderer, int x, int y, int w, int h) {
    // Play panel close animation
}

void VisualFeedback::PlaySuccessEffect(GUIRenderer* renderer, int x, int y) {
    // Play success visual effect
}

void VisualFeedback::PlayFailureEffect(GUIRenderer* renderer, int x, int y) {
    // Play failure visual effect
}

void VisualFeedback::PlayWarningEffect(GUIRenderer* renderer, int x, int y) {
    // Play warning visual effect
}

ParticleEffect::ParticleEffect(int x, int y, int count, const Color& color)
    : originX_(x), originY_(y) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * 3.14159f);
    std::uniform_real_distribution<float> speedDist(20.0f, 80.0f);
    std::uniform_real_distribution<float> lifeDist(0.5f, 1.5f);
    std::uniform_real_distribution<float> sizeDist(2.0f, 6.0f);

    for (int i = 0; i < count; ++i) {
        Particle p;
        p.x = static_cast<float>(x);
        p.y = static_cast<float>(y);
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;
        p.life = p.maxLife = lifeDist(gen);
        p.r = color.r;
        p.g = color.g;
        p.b = color.b;
        p.a = color.a;
        p.size = sizeDist(gen);
        particles_.push_back(p);
    }
}

void ParticleEffect::Update(float deltaTime) {
    for (auto& p : particles_) {
        if (p.life > 0.0f) {
            p.x += p.vx * deltaTime;
            p.y += p.vy * deltaTime;
            p.vy += 50.0f * deltaTime; // Gravity
            p.life -= deltaTime;
            p.a = static_cast<uint8_t>(255.0f * (p.life / p.maxLife));
        }
    }
}

void ParticleEffect::Render(GUIRenderer* renderer) {
    for (const auto& p : particles_) {
        if (p.life > 0.0f) {
            renderer->SetDrawColor(Color{p.r, p.g, p.b, p.a});
            renderer->DrawRect(static_cast<int>(p.x - p.size/2), static_cast<int>(p.y - p.size/2),
                               static_cast<int>(p.size), static_cast<int>(p.size), true);
        }
    }
}

bool ParticleEffect::IsComplete() const {
    for (const auto& p : particles_) {
        if (p.life > 0.0f) return false;
    }
    return true;
}

ScreenTransition::ScreenTransition(Type type, float duration)
    : type_(type), duration_(duration) {}

void ScreenTransition::Update(float deltaTime) {
    if (!started_) return;

    elapsed_ += deltaTime;
    progress_ = std::min(1.0f, elapsed_ / duration_);
}

void ScreenTransition::Render(GUIRenderer* renderer) {
    if (!started_ || progress_ >= 1.0f) return;

    int w = renderer->GetWidth();
    int h = renderer->GetHeight();

    float t = EaseInOut(progress_);

    switch (type_) {
        case Type::FadeToBlack:
        case Type::FadeFromBlack: {
            uint8_t alpha = static_cast<uint8_t>(255.0f * t);
            renderer->SetDrawColor(Color{0, 0, 0, alpha});
            renderer->DrawRect(0, 0, w, h, true);
            break;
        }
        case Type::SlideLeft: {
            int offset = static_cast<int>(w * t);
            renderer->SetDrawColor(Color::Background);
            renderer->DrawRect(w - offset, 0, offset, h, true);
            break;
        }
        case Type::SlideRight: {
            int offset = static_cast<int>(w * t);
            renderer->SetDrawColor(Color::Background);
            renderer->DrawRect(0, 0, offset, h, true);
            break;
        }
        case Type::WipeHorizontal: {
            int wipeWidth = static_cast<int>(w * t);
            renderer->SetDrawColor(Color::Primary);
            renderer->DrawRect((w - wipeWidth) / 2, 0, wipeWidth, h, true);
            break;
        }
        case Type::WipeVertical: {
            int wipeHeight = static_cast<int>(h * t);
            renderer->SetDrawColor(Color::Primary);
            renderer->DrawRect(0, (h - wipeHeight) / 2, w, wipeHeight, true);
            break;
        }
        default:
            break;
    }
}

void ScreenTransition::Start() {
    started_ = true;
    elapsed_ = 0.0f;
    progress_ = 0.0f;
}

void ScreenTransition::Reset() {
    started_ = false;
    elapsed_ = 0.0f;
    progress_ = 0.0f;
}

float Ease(EasingType type, float t) {
    switch (type) {
        case EasingType::Linear: return t;
        case EasingType::EaseIn: return EaseIn(t);
        case EasingType::EaseOut: return EaseOut(t);
        case EasingType::EaseInOut: return EaseInOut(t);
        case EasingType::Bounce: return Bounce(t);
        case EasingType::Elastic: return Elastic(t);
        default: return t;
    }
}

float EaseIn(float t) {
    return t * t;
}

float EaseOut(float t) {
    return 1.0f - (1.0f - t) * (1.0f - t);
}

float EaseInOut(float t) {
    return t < 0.5f ? 2.0f * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

float Bounce(float t) {
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    } else if (t < 2.0f / 2.75f) {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    } else if (t < 2.5f / 2.75f) {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    } else {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
}

float Elastic(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return -std::pow(2.0f, 10.0f * (t - 1.0f)) * std::sin((t - 1.1f) * 5.0f * 3.14159f);
}

}
