#include "GUIComponents.h"
#include "GUIRenderer.h"
#include <SDL2/SDL.h>

namespace PowerGame {

GUIComponent::GUIComponent(int x, int y, int width, int height)
    : x_(x), y_(y), width_(width), height_(height) {}

GUIButton::GUIButton(int x, int y, int width, int height, const std::string& text)
    : GUIComponent(x, y, width, height), text_(text) {}

void GUIButton::Render(GUIRenderer* renderer) {
    if (!visible_) return;
    
    Color currentColor = normalColor_;
    if (!enabled_) {
        currentColor = Color::SurfaceLow;
    } else if (isPressed_) {
        currentColor = pressedColor_;
    } else if (isHovered_) {
        currentColor = hoverColor_;
    }
    
    renderer->SetDrawColor(currentColor);
    renderer->DrawRect(x_, y_, width_, height_, true);
    
    renderer->SetDrawColor(Color::Primary);
    renderer->DrawRect(x_, y_, width_, height_, false);
    
    int textX = x_ + (width_ - static_cast<int>(text_.length()) * 10) / 2;
    int textY = y_ + (height_ - 16) / 2;
    renderer->DrawText(text_, textX, textY, nullptr, Color::TextPrimary);
}

bool GUIButton::HandleEvent(const SDL_Event& event) {
    if (!enabled_ || !visible_) return false;
    
    if (event.type == SDL_MOUSEMOTION) {
        isHovered_ = ContainsPoint(event.motion.x, event.motion.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (ContainsPoint(event.button.x, event.button.y)) {
            isPressed_ = true;
        }
    } else if (event.type == SDL_MOUSEBUTTONUP) {
        if (isPressed_ && ContainsPoint(event.button.x, event.button.y)) {
            if (clickHandler_) {
                clickHandler_();
            }
        }
        isPressed_ = false;
    }
    
    return false;
}

void GUIButton::SetStyle(const Color& normal, const Color& hover, const Color& pressed) {
    normalColor_ = normal;
    hoverColor_ = hover;
    pressedColor_ = pressed;
}

GUILabel::GUILabel(int x, int y, const std::string& text, TTF_Font* font)
    : GUIComponent(x, y, 0, 0), text_(text), font_(font) {}

void GUILabel::Render(GUIRenderer* renderer) {
    if (!visible_) return;
    renderer->DrawText(text_, x_, y_, font_, color_);
}

bool GUILabel::HandleEvent(const SDL_Event& event) {
    return false;
}

GUIProgressBar::GUIProgressBar(int x, int y, int width, int height)
    : GUIComponent(x, y, width, height) {}

void GUIProgressBar::Render(GUIRenderer* renderer) {
    if (!visible_) return;
    
    renderer->DrawProgressBar(x_, y_, width_, height_, progress_, fillColor_, bgColor_);
    
    if (!label_.empty()) {
        int textX = x_ + 5;
        int textY = y_ + (height_ - 14) / 2;
        renderer->DrawText(label_, textX, textY, nullptr, Color::TextPrimary);
    }
}

bool GUIProgressBar::HandleEvent(const SDL_Event& event) {
    return false;
}

GUIPanel::GUIPanel(int x, int y, int width, int height)
    : GUIComponent(x, y, width, height) {}

void GUIPanel::Render(GUIRenderer* renderer) {
    if (!visible_) return;
    
    renderer->DrawPanel(x_, y_, width_, height_, bgColor_);
    
    for (auto child : children_) {
        child->Render(renderer);
    }
}

bool GUIPanel::HandleEvent(const SDL_Event& event) {
    for (auto child : children_) {
        if (child->HandleEvent(event)) {
            return true;
        }
    }
    return false;
}

void GUIPanel::AddChild(GUIComponent* child) {
    children_.push_back(child);
}

void GUIPanel::ClearChildren() {
    children_.clear();
}

GUIImage::GUIImage(int x, int y, int width, int height, const std::string& imagePath)
    : GUIComponent(x, y, width, height), imagePath_(imagePath) {
    texture_ = TextureManager::Instance().LoadTexture(imagePath);
}

void GUIImage::Render(GUIRenderer* renderer) {
    if (!visible_ || !texture_) return;
    renderer->DrawTexture(texture_, x_, y_, width_, height_);
}

bool GUIImage::HandleEvent(const SDL_Event& event) {
    return false;
}

void GUIImage::SetImagePath(const std::string& path) {
    imagePath_ = path;
    texture_ = TextureManager::Instance().LoadTexture(path);
}

GUIListBox::GUIListBox(int x, int y, int width, int height)
    : GUIComponent(x, y, width, height) {
    visibleItems_ = height / 24;
}

GUIListBox::~GUIListBox() {
    for (auto child : children_) {
        delete child;
    }
    children_.clear();
}

void GUIListBox::Render(GUIRenderer* renderer) {
    if (!visible_) return;
    
    renderer->SetDrawColor(Color::SurfaceLow);
    renderer->DrawRect(x_, y_, width_, height_, true);
    
    renderer->SetDrawColor(Color::Outline);
    renderer->DrawRect(x_, y_, width_, height_, false);
    
    int itemHeight = 24;
    int startY = y_ + 2;
    
    for (size_t i = scrollOffset_; i < items_.size() && i < scrollOffset_ + static_cast<size_t>(visibleItems_); ++i) {
        int itemY = startY + static_cast<int>(i - scrollOffset_) * itemHeight;
        
        if (static_cast<int>(i) == selectedIndex_) {
            renderer->SetDrawColor(Color::Primary);
            renderer->DrawRect(x_ + 2, itemY, width_ - 4, itemHeight - 2, true);
        }
        
        renderer->DrawText(items_[i], x_ + 10, itemY + 4, nullptr, Color::TextPrimary);
    }
}

bool GUIListBox::HandleEvent(const SDL_Event& event) {
    if (!enabled_ || !visible_) return false;
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mx = event.button.x;
        int my = event.button.y;
        
        if (mx >= x_ && mx <= x_ + width_ && my >= y_ && my <= y_ + height_) {
            int itemIndex = (my - y_ - 2) / 24 + scrollOffset_;
            if (itemIndex >= 0 && itemIndex < static_cast<int>(items_.size())) {
                SetSelectedIndex(itemIndex);
                if (selectHandler_) {
                    selectHandler_(itemIndex);
                }
                return true;
            }
        }
    } else if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0) {
            scrollOffset_ = std::max(0, scrollOffset_ - 1);
        } else if (event.wheel.y < 0) {
            scrollOffset_ = std::min(
                static_cast<int>(items_.size()) - visibleItems_,
                scrollOffset_ + 1
            );
        }
        return true;
    }
    
    return false;
}

void GUIListBox::AddItem(const std::string& item) {
    items_.push_back(item);
}

void GUIListBox::ClearItems() {
    items_.clear();
    selectedIndex_ = -1;
}

void GUIListBox::SetSelectedIndex(int index) {
    if (index >= 0 && index < static_cast<int>(items_.size())) {
        selectedIndex_ = index;
        if (selectedIndex_ < scrollOffset_) {
            scrollOffset_ = selectedIndex_;
        } else if (selectedIndex_ >= scrollOffset_ + visibleItems_) {
            scrollOffset_ = selectedIndex_ - visibleItems_ + 1;
        }
    }
}

}
