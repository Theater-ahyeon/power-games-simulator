#include "GUIRenderer.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include <algorithm>

namespace PowerGame {

const Color Color::Background = Color::FromHex(0x111316FF);
const Color Color::SurfaceLowest = Color::FromHex(0x0c0e11FF);
const Color Color::SurfaceLow = Color::FromHex(0x1a1c20FF);
const Color Color::Surface = Color::FromHex(0x111316FF);
const Color Color::SurfaceHigh = Color::FromHex(0x222529FF);
const Color Color::Primary = Color::FromHex(0x00daf3FF);
const Color Color::Secondary = Color::FromHex(0xe9c349FF);
const Color Color::Tertiary = Color::FromHex(0xffb4acFF);
const Color Color::TextPrimary = Color::FromHex(0xffffffff);
const Color Color::TextSecondary = Color::FromHex(0xc4c6cfFF);
const Color Color::Outline = Color::FromHex(0x45464dFF);

TextureManager& TextureManager::Instance() {
    static TextureManager instance;
    return instance;
}

SDL_Texture* TextureManager::LoadTexture(const std::string& path) {
    if (textures_.find(path) != textures_.end()) {
        return textures_[path];
    }
    
    SDL_Texture* texture = IMG_LoadTexture(renderer_, path.c_str());
    if (texture) {
        textures_[path] = texture;
    }
    return texture;
}

SDL_Texture* TextureManager::CreateTextTexture(const std::string& text, TTF_Font* font, const Color& color) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) return nullptr;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void TextureManager::Clear() {
    for (auto& pair : textures_) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    textures_.clear();
}

GUIRenderer::GUIRenderer() : width_(1280), height_(720) {}

GUIRenderer::~GUIRenderer() {
    Shutdown();
}

bool GUIRenderer::Initialize(int width, int height) {
    width_ = width;
    height_ = height;
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        return false;
    }
    
    if (TTF_Init() < 0) {
        return false;
    }
    
    window_ = SDL_CreateWindow(
        "Power Game: The Art of Political Intrigue",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width_,
        height_,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (!window_) {
        return false;
    }
    
    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        return false;
    }
    
    TextureManager::Instance().SetRenderer(renderer_);
    
    defaultFont_ = TTF_OpenFont("Content/Fonts/Roboto-Regular.ttf", 16);
    if (!defaultFont_) {
        defaultFont_ = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 16);
    }
    
    return true;
}

void GUIRenderer::Shutdown() {
    if (defaultFont_) {
        TTF_CloseFont(defaultFont_);
        defaultFont_ = nullptr;
    }
    
    TextureManager::Instance().Clear();
    
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
}

void GUIRenderer::Clear() {
    SDL_SetRenderDrawColor(renderer_, Background.r, Background.g, Background.b, Background.a);
    SDL_RenderClear(renderer_);
}

void GUIRenderer::Present() {
    SDL_RenderPresent(renderer_);
}

void GUIRenderer::SetDrawColor(const Color& color) {
    SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
}

void GUIRenderer::DrawRect(int x, int y, int w, int h, bool filled) {
    if (filled) {
        SDL_RenderFillRect(renderer_, &(SDL_Rect{x, y, w, h}));
    } else {
        SDL_RenderDrawRect(renderer_, &(SDL_Rect{x, y, w, h}));
    }
}

void GUIRenderer::DrawLine(int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(renderer_, x1, y1, x2, y2);
}

void GUIRenderer::DrawText(const std::string& text, int x, int y, TTF_Font* font, const Color& color) {
    if (!font) font = defaultFont_;
    if (!font) return;
    
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, surface);
    SDL_Rect dstRect = {x, y, surface->w, surface->h};
    
    SDL_RenderCopy(renderer_, texture, nullptr, &dstRect);
    
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void GUIRenderer::DrawTexture(SDL_Texture* texture, int x, int y, int w, int h) {
    if (!texture) return;
    
    int texW, texH;
    SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);
    
    if (w == 0) w = texW;
    if (h == 0) h = texH;
    
    SDL_Rect dstRect = {x, y, w, h};
    SDL_RenderCopy(renderer_, texture, nullptr, &dstRect);
}

void GUIRenderer::DrawProgressBar(int x, int y, int w, int h, float progress, const Color& fillColor, const Color& bgColor) {
    SetDrawColor(bgColor);
    DrawRect(x, y, w, h, true);
    
    int fillW = static_cast<int>(w * progress);
    if (fillW > 0) {
        SetDrawColor(fillColor);
        DrawRect(x, y, fillW, h, true);
    }
    
    SetDrawColor(Color::Outline);
    DrawRect(x, y, w, h, false);
}

void GUIRenderer::DrawPanel(int x, int y, int w, int h, const Color& bgColor) {
    SetDrawColor(bgColor);
    DrawRect(x, y, w, h, true);
}

}
