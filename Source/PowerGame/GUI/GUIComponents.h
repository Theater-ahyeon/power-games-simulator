#pragma once
#include <SDL2/SDL.h>
#include <string>
#include <functional>
#include <vector>
#include "GUIRenderer.h"

namespace PowerGame {

class GUIComponent {
public:
    GUIComponent(int x, int y, int width, int height);
    virtual ~GUIComponent() = default;
    
    virtual void Render(GUIRenderer* renderer) = 0;
    virtual bool HandleEvent(const SDL_Event& event) = 0;
    
    void SetPosition(int x, int y) { x_ = x; y_ = y; }
    void SetSize(int w, int h) { width_ = w; height_ = h; }
    void SetVisible(bool visible) { visible_ = visible; }
    void SetEnabled(bool enabled) { enabled_ = enabled; }
    
    bool IsVisible() const { return visible_; }
    bool IsEnabled() const { return enabled_; }
    int GetX() const { return x_; }
    int GetY() const { return y_; }
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }
    
    bool ContainsPoint(int px, int py) const {
        return px >= x_ && px <= x_ + width_ && py >= y_ && py <= y_ + height_;
    }

protected:
    int x_, y_, width_, height_;
    bool visible_ = true;
    bool enabled_ = true;
};

class GUIButton : public GUIComponent {
public:
    using ClickHandler = std::function<void()>;
    
    GUIButton(int x, int y, int width, int height, const std::string& text);
    
    void Render(GUIRenderer* renderer) override;
    bool HandleEvent(const SDL_Event& event) override;
    
    void SetText(const std::string& text) { text_ = text; }
    void SetClickHandler(ClickHandler handler) { clickHandler_ = handler; }
    void SetStyle(const Color& normal, const Color& hover, const Color& pressed);

private:
    std::string text_;
    ClickHandler clickHandler_;
    Color normalColor_ = Color::SurfaceHigh;
    Color hoverColor_ = Color::Primary;
    Color pressedColor_ = Color::FromHex(0x008e9fFF);
    bool isHovered_ = false;
    bool isPressed_ = false;
};

class GUILabel : public GUIComponent {
public:
    GUILabel(int x, int y, const std::string& text, TTF_Font* font = nullptr);
    
    void Render(GUIRenderer* renderer) override;
    bool HandleEvent(const SDL_Event& event) override;
    
    void SetText(const std::string& text) { text_ = text; }
    void SetFont(TTF_Font* font) { font_ = font; }
    void SetColor(const Color& color) { color_ = color; }

private:
    std::string text_;
    TTF_Font* font_;
    Color color_ = Color::TextPrimary;
};

class GUIProgressBar : public GUIComponent {
public:
    GUIProgressBar(int x, int y, int width, int height);
    
    void Render(GUIRenderer* renderer) override;
    bool HandleEvent(const SDL_Event& event) override;
    
    void SetProgress(float progress) { progress_ = std::max(0.0f, std::min(1.0f, progress)); }
    void SetFillColor(const Color& color) { fillColor_ = color; }
    void SetLabel(const std::string& label) { label_ = label; }

private:
    float progress_ = 0.0f;
    Color fillColor_ = Color::Primary;
    Color bgColor_ = Color::SurfaceLow;
    std::string label_;
};

class GUIPanel : public GUIComponent {
public:
    GUIPanel(int x, int y, int width, int height);
    
    void Render(GUIRenderer* renderer) override;
    bool HandleEvent(const SDL_Event& event) override;
    
    void SetBackgroundColor(const Color& color) { bgColor_ = color; }
    void AddChild(GUIComponent* child) { children_.push_back(child); }
    void ClearChildren();

private:
    Color bgColor_ = Color::SurfaceLow;
    std::vector<GUIComponent*> children_;
};

class GUIImage : public GUIComponent {
public:
    GUIImage(int x, int y, int width, int height, const std::string& imagePath);
    
    void Render(GUIRenderer* renderer) override;
    bool HandleEvent(const SDL_Event& event) override;
    
    void SetImagePath(const std::string& path);

private:
    std::string imagePath_;
    SDL_Texture* texture_ = nullptr;
};

class GUIListBox : public GUIComponent {
public:
    using SelectHandler = std::function<void(int index)>;
    
    GUIListBox(int x, int y, int width, int height);
    ~GUIListBox() override;
    
    void Render(GUIRenderer* renderer) override;
    bool HandleEvent(const SDL_Event& event) override;
    
    void AddItem(const std::string& item);
    void ClearItems();
    int GetSelectedIndex() const { return selectedIndex_; }
    void SetSelectedIndex(int index);
    void SetSelectHandler(SelectHandler handler) { selectHandler_ = handler; }

private:
    std::vector<std::string> items_;
    int selectedIndex_ = -1;
    int scrollOffset_ = 0;
    int visibleItems_ = 0;
    SelectHandler selectHandler_;
};

}
