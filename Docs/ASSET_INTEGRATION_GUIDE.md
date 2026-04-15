# Power Game 资源整合与开发指南

## 项目概述

**Power Game: The Art of Political Intrigue** 是一款以权力斗争为核心机制的深度策略游戏，基于主权情报设计系统（The Sovereign Intelligence Design System）进行视觉和交互设计。

## 设计系统规范

### 色彩体系

| 名称 | 颜色代码 | 用途 |
|------|----------|------|
| Background | `#111316` | 主背景色 |
| Surface Lowest | `#0c0e11` | 最深层面板 |
| Surface Low | `#1a1c20` | 次级面板 |
| Surface High | `#222529` | 高亮面板 |
| Primary (Cyan) | `#00daf3` | 主要交互元素 |
| Secondary (Gold) | `#e9c349` | 重要/威望元素 |
| Tertiary (Warning) | `#ffb4ac` | 警告/冲突 |
| Text Primary | `#ffffff` | 主要文本 |
| Text Secondary | `#c4c6cf` | 次要文本 |
| Outline | `#45464d` | 边框/分隔 |

### 设计原则

1. **零圆角设计** - 所有UI元素采用0px圆角
2. **深色背景** - 使用#111316和#0c0e11
3. **高对比度** - 青色和金色强调系统
4. **左对齐文本** - 专业文档风格
5. **无分隔线** - 通过颜色对比实现视觉分离
6. **建筑不对称** - 模块化数据集群布局

## 资源目录结构

```
PowerGame/
├── Content/
│   ├── Art/
│   │   ├── Characters/           # 角色肖像
│   │   │   ├── chen_weiming.png
│   │   │   ├── sarah_jenkins.png
│   │   │   └── ...
│   │   ├── Scenes/               # 场景背景
│   │   │   ├── war_room.png
│   │   │   ├── parliament.png
│   │   │   └── ...
│   │   ├── UI/                  # UI元素
│   │   │   ├── main_dashboard.txt
│   │   │   ├── resource_economy.txt
│   │   │   └── ...
│   │   ├── Icons/                # 图标资源
│   │   │   └── *.png
│   │   └── Prototypes/            # HTML原型
│   │       ├── character_prototype.html
│   │       └── scene_ui_prototype.html
│   └── Audio/                    # 音频资源
│       ├── bgm_title.mp3
│       ├── bgm_gameplay.mp3
│       ├── sfx_*.mp3
│       └── audio_config.json
├── Tools/
│   └── AssetGenerator/
│       ├── minimax_asset_generator.py
│       └── game_asset_generator.py
└── Source/
    └── PowerGame/
        ├── GUI/
        ├── Audio/
        └── ...
```

## 资源生成

### 使用MiniMax API生成资源

1. **设置API密钥**
```bash
export MINIMAX_API_KEY="your-api-key-here"
```

2. **运行资源生成器**
```bash
cd Tools/AssetGenerator
python game_asset_generator.py
```

3. **资源类型**
- 角色肖像 (1024x1024, PNG, 1:1)
- 场景背景 (1920x1080, PNG/JPG, 16:9)
- UI图标 (256x256, PNG, 透明背景)
- 背景音乐 (MP3, 44.1kHz, 192kbps)
- 音效 (MP3/WAV, 44.1kHz)
- 语音 (MP3, 22.05kHz)

### 手动资源放置

将生成的资源放置在对应目录：
- 角色肖像 → `Content/Art/Characters/`
- 场景 → `Content/Art/Scenes/`
- 图标 → `Content/Art/Icons/`
- 音乐 → `Content/Audio/`
- 原型 → `Content/Art/Prototypes/`

## 开发框架集成

### CMake配置

确保CMakeLists.txt包含所有源文件：

```cmake
file(GLOB_RECURSE POWERGAME_SOURCES
    "Source/PowerGame/GUI/*.cpp"
    "Source/PowerGame/Audio/*.cpp"
    # ... 其他模块
)
```

### SDL2依赖

```cmake
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

target_link_libraries(PowerGame PRIVATE
    SDL2::SDL2
    SDL2::SDL2_mixer
    SDL2::SDL2_image
    SDL2::SDL2_ttf
)
```

### 资源加载示例

```cpp
#include <SDL2/SDL_image.h>

SDL_Texture* LoadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) return nullptr;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
```

### 音频播放示例

```cpp
#include <SDL2/SDL_mixer.h>

void PlayBackgroundMusic(const std::string& path) {
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (music) {
        Mix_PlayMusic(music, -1);
        Mix_VolumeMusic(64); // 50% volume
    }
}

void PlaySoundEffect(const std::string& path) {
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (sound) {
        Mix_PlayChannel(-1, sound, 0);
    }
}
```

## 性能优化

### 纹理管理
- 使用纹理图集（Spritesheet）
- 实现纹理缓存
- 及时释放不需要的纹理

### 音频优化
- 预加载背景音乐
- 使用音效池管理
- 实现音频淡入淡出

### 渲染优化
- 使用批量渲染
- 避免不必要的重绘
- 60 FPS帧率控制

## 测试

### 单元测试
```bash
cd build
ctest --output-on-failure
```

### 原型测试
在浏览器中打开HTML原型文件验证设计系统：
- `Content/Art/Prototypes/character_prototype.html`
- `Content/Art/Prototypes/scene_ui_prototype.html`

## 版本控制

### Git提交规范
```
[类型] 描述

类型:
- feat: 新功能
- fix: 修复
- docs: 文档
- style: 格式调整
- refactor: 重构
- test: 测试
- chore: 构建/工具
```

### 示例
```bash
git add .
git commit -m "feat: 添加角色肖像生成系统和HTML原型"
git push origin main
```

## 相关资源

- [主权情报设计系统](stitch_political_intrigue_simulator/stitch_political_intrigue_simulator/midnight_protocol/DESIGN.md)
- [API参考文档](Docs/API_REFERENCE.md)
- [MiniMax API文档](https://platform.minimax.chat/)

## 许可证

MIT License - 详见 LICENSE 文件
