#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "../Core/GameSystem.h"

namespace PowerGame {

struct Color {
    uint8_t r, g, b, a;
    
    static Color FromHex(uint32_t hex) {
        return {
            static_cast<uint8_t>((hex >> 24) & 0xFF),
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF)
        };
    }
    
    static const Color Background;      // #111316
    static const Color SurfaceLowest;   // #0c0e11
    static const Color SurfaceLow;      // #1a1c20
    static const Color Surface;          // #111316
    static const Color SurfaceHigh;     // #222529
    static const Color Primary;         // #00daf3 (青色)
    static const Color Secondary;        // #e9c349 (金色)
    static const Color Tertiary;         // #ffb4ac (绯红)
    static const Color TextPrimary;      // #ffffff
    static const Color TextSecondary;    // #c4c6cf
    static const Color Outline;          // #45464d
};

class TextureManager {
public:
    static TextureManager& Instance();
    
    SDL_Texture* LoadTexture(const std::string& path);
    SDL_Texture* CreateTextTexture(const std::string& text, TTF_Font* font, const Color& color);
    void SetRenderer(SDL_Renderer* renderer) { renderer_ = renderer; }
    
    void Clear();
    
private:
    TextureManager() = default;
    SDL_Renderer* renderer_ = nullptr;
    std::unordered_map<std::string, SDL_Texture*> textures_;
};

class GUIRenderer {
public:
    GUIRenderer();
    ~GUIRenderer();
    
    bool Initialize(int width, int height);
    void Shutdown();
    
    void Clear();
    void Present();
    
    void SetDrawColor(const Color& color);
    void DrawRect(int x, int y, int w, int h, bool filled = true);
    void DrawLine(int x1, int y1, int x2, int y2);
    void DrawText(const std::string& text, int x, int y, TTF_Font* font, const Color& color);
    void DrawTexture(SDL_Texture* texture, int x, int y, int w = 0, int h = 0);
    
    void DrawProgressBar(int x, int y, int w, int h, float progress, const Color& fillColor, const Color& bgColor);
    void DrawPanel(int x, int y, int w, int h, const Color& bgColor);
    
    SDL_Renderer* GetSDLRenderer() const { return renderer_; }
    SDL_Window* GetWindow() const { return window_; }
    
    int GetWidth() const { return width_; }
    int GetHeight() const { return height_; }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    TTF_Font* defaultFont_ = nullptr;
    int width_;
    int height_;
};

}
