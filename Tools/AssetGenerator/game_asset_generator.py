"""
Power Game - Complete Asset Generator
基于主权情报设计系统的游戏资源生成器

设计规范：
- 色彩体系：深色背景 (#111316, #0c0e11)
- 强调色：青色 (#00daf3)、金色 (#e9c349)、绯红 (#ffb4ac)
- 零圆角设计
- 左对齐文本
- 硬边界面
"""

import json
import os
import base64
import requests
import time
from typing import Dict, List, Optional
from pathlib import Path

class PowerGameAssetGenerator:
    """Power Game资源生成器"""
    
    DESIGN_SYSTEM = {
        "colors": {
            "background": "#111316",
            "surface_lowest": "#0c0e11",
            "surface_low": "#1a1c20",
            "surface": "#111316",
            "surface_high": "#222529",
            "primary": "#00daf3",
            "secondary": "#e9c349",
            "tertiary": "#ffb4ac",
            "text_primary": "#ffffff",
            "text_secondary": "#c4c6cf",
            "outline": "#45464d"
        },
        "typography": {
            "display": "Newsreader",
            "body": "Inter", 
            "data": "Space Grotesk"
        },
        "design_principles": [
            "零圆角设计",
            "深色背景",
            "高对比度",
            "左对齐文本",
            "无分隔线",
            "建筑不对称布局"
        ]
    }
    
    def __init__(self, api_key: str, output_dir: str = "Content/Art"):
        self.api_key = api_key
        self.base_url = "https://api.minimax.chat/v1"
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(parents=True, exist_ok=True)
        
        # 创建子目录
        self.characters_dir = self.output_dir / "Characters"
        self.scenes_dir = self.output_dir / "Scenes"
        self.ui_dir = self.output_dir / "UI"
        self.audio_dir = self.output_dir / "Audio"
        self.icons_dir = self.output_dir / "Icons"
        
        for d in [self.characters_dir, self.scenes_dir, self.ui_dir, 
                  self.audio_dir, self.icons_dir]:
            d.mkdir(parents=True, exist_ok=True)
    
    def _make_request(self, endpoint: str, payload: dict) -> Optional[dict]:
        """发起API请求"""
        headers = {
            "Authorization": f"Bearer {self.api_key}",
            "Content-Type": "application/json"
        }
        
        try:
            response = requests.post(
                f"{self.base_url}/{endpoint}",
                headers=headers,
                json=payload,
                timeout=120
            )
            response.raise_for_status()
            return response.json()
        except Exception as e:
            print(f"API请求失败: {e}")
            return None
    
    def generate_character_portrait(self, character: Dict) -> Optional[str]:
        """生成角色肖像"""
        prompt = self._build_character_prompt(character)
        
        payload = {
            "model": "image-01",
            "prompt": prompt,
            "num_images": 1,
            "aspect_ratio": "1:1",
            "response_format": "base64"
        }
        
        result = self._make_request("images/generations", payload)
        
        if result and "data" in result:
            image_data = result["data"][0]["b64_json"]
            output_path = self.characters_dir / f"{character['id']}.png"
            
            with open(output_path, "wb") as f:
                f.write(base64.b64decode(image_data))
            
            print(f"✓ 生成角色肖像: {output_path}")
            return str(output_path)
        
        return None
    
    def _build_character_prompt(self, character: Dict) -> str:
        """构建角色图像提示词"""
        faction_descriptions = {
            "conservative": "traditional East Asian politician, formal dark suit, authoritative presence",
            "liberal": "modern progressive politician, business casual attire, intellectual appearance",
            "neutral": "diplomatic official, balanced appearance, mediator presence",
            "radical": "revolutionary figure, military style jacket, intense expression",
            "opportunist": "mysterious shadowy figure, ambiguous features, calculating gaze"
        }
        
        faction = character.get("faction", "neutral")
        faction_desc = faction_descriptions.get(faction.lower(), "")
        
        prompt = f"""
        Character portrait: {character['name']}
        
        Style: Political thriller aesthetic
        - Dark atmospheric lighting
        - Professional headshot composition
        - Color scheme: Deep grays (#111316, #222529) with cyan accents (#00daf3)
        - Gold highlights for important elements (#e9c349)
        - Sharp edges, no rounded corners
        - Cinematic quality
        
        Character: {faction_desc}
        
        Requirements:
        - 1:1 aspect ratio
        - Clear facial features
        - Professional headshot style
        - Dark moody background
        - Cyan/gold accent lighting
        - High contrast
        """
        
        return prompt.strip()
    
    def generate_scene(self, scene: Dict) -> Optional[str]:
        """生成场景背景"""
        prompt = self._build_scene_prompt(scene)
        
        payload = {
            "model": "image-01",
            "prompt": prompt,
            "num_images": 1,
            "aspect_ratio": "16:9",
            "response_format": "base64"
        }
        
        result = self._make_request("images/generations", payload)
        
        if result and "data" in result:
            image_data = result["data"][0]["b64_json"]
            output_path = self.scenes_dir / f"{scene['id']}.png"
            
            with open(output_path, "wb") as f:
                f.write(base64.b64decode(image_data))
            
            print(f"✓ 生成场景: {output_path}")
            return str(output_path)
        
        return None
    
    def _build_scene_prompt(self, scene: Dict) -> str:
        """构建场景提示词"""
        scene_descriptions = {
            "war_room": "High-tech command center, multiple screens showing political data and maps, dark atmosphere with cyan lighting",
            "parliament": "Grand parliamentary chamber, dramatic architecture, rows of seats, governmental power",
            "mansion": "Luxurious private mansion, marble floors, expensive art, wealth display",
            "underground_bunker": "Underground secure room, concrete walls, emergency lighting, tactical planning",
            "office": "Executive office, dark wood panels, authoritative desk, power symbols",
            "conference": "Conference room, large table, strategic planning atmosphere"
        }
        
        scene_type = scene.get("type", "war_room")
        base_desc = scene_descriptions.get(scene_type, "")
        
        prompt = f"""
        Scene: {scene['name']}
        
        Style: Political thriller aesthetic
        - Dark atmospheric environment
        - Color scheme: #111316, #0c0e11, #222529
        - Cyan accent lighting (#00daf3)
        - Gold highlights (#e9c349)
        - Sharp architectural elements
        - Cinematic composition
        
        Scene type: {base_desc}
        
        Requirements:
        - 16:9 aspect ratio
        - Wide establishing shot
        - Moody atmospheric lighting
        - Detailed architectural elements
        - Political power symbolism
        - Professional quality
        """
        
        return prompt.strip()
    
    def generate_ui_icon(self, icon: Dict) -> Optional[str]:
        """生成UI图标"""
        prompt = self._build_icon_prompt(icon)
        
        payload = {
            "model": "image-01",
            "prompt": prompt,
            "num_images": 1,
            "aspect_ratio": "1:1",
            "response_format": "base64"
        }
        
        result = self._make_request("images/generations", payload)
        
        if result and "data" in result:
            image_data = result["data"][0]["b64_json"]
            output_path = self.icons_dir / f"{icon['id']}.png"
            
            with open(output_path, "wb") as f:
                f.write(base64.b64decode(image_data))
            
            print(f"✓ 生成图标: {output_path}")
            return str(output_path)
        
        return None
    
    def _build_icon_prompt(self, icon: Dict) -> str:
        """构建图标提示词"""
        icon_types = {
            "resource": "Minimalist icon representing political resource or power",
            "faction": "Icon for political faction or organization",
            "action": "Icon for game action or decision",
            "status": "Status indicator or badge",
            "navigation": "Navigation or menu icon"
        }
        
        icon_type = icon.get("type", "resource")
        base_desc = icon_types.get(icon_type, "")
        
        prompt = f"""
        UI Icon: {icon['name']}
        
        Style: Minimalist flat design
        - Color scheme: #00daf3 (cyan), #e9c349 (gold), #ffb4ac (warning red)
        - Transparent background
        - Sharp edges, no rounded corners
        - Clean minimalist design
        - Political/power theme elements
        
        Icon type: {base_desc}
        
        Requirements:
        - 1:1 aspect ratio
        - Minimalist flat design
        - Clear silhouette
        - Readable at small sizes
        - Single color or limited palette
        - High contrast
        """
        
        return prompt.strip()
    
    def generate_tts(self, text: str, output_path: str, voice: str = "default") -> Optional[str]:
        """生成语音"""
        # 这里需要调用MiniMax TTS API
        # 简化版本：生成配置文件
        config = {
            "text": text,
            "voice": voice,
            "output": output_path,
            "settings": {
                "speed": 1.0,
                "pitch": 0,
                "volume": 1.0
            }
        }
        
        config_path = Path(output_path).with_suffix('.json')
        with open(config_path, 'w', encoding='utf-8') as f:
            json.dump(config, f, indent=2, ensure_ascii=False)
        
        print(f"✓ TTS配置已创建: {config_path}")
        return str(config_path)
    
    def generate_music(self, music_config: Dict) -> Optional[str]:
        """生成背景音乐"""
        # 这里需要调用MiniMax Music API
        # 简化版本：生成配置文件
        config = {
            "name": music_config.get("name", "Untitled"),
            "style": music_config.get("style", "instrumental"),
            "mood": music_config.get("mood", "neutral"),
            "duration": music_config.get("duration", 60),
            "settings": {
                "tempo": "medium",
                "key": "minor",
                "instruments": ["piano", "strings", "drums"]
            }
        }
        
        output_path = self.audio_dir / f"{music_config.get('id', 'untitled')}_config.json"
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(config, f, indent=2, ensure_ascii=False)
        
        print(f"✓ 音乐配置已创建: {output_path}")
        return str(output_path)


def main():
    api_key = os.environ.get("MINIMAX_API_KEY", "")
    
    if not api_key:
        print("错误: 请设置MINIMAX_API_KEY环境变量")
        return
    
    generator = PowerGameAssetGenerator(api_key)
    
    # 角色数据
    characters = [
        {"id": "chen_weiming", "name": "Chen Weiming", "faction": "conservative"},
        {"id": "sarah_jenkins", "name": "Sarah Jenkins", "faction": "liberal"},
        {"id": "minister_zhang", "name": "Minister Zhang", "faction": "neutral"},
        {"id": "commander_fire", "name": "Commander Fire", "faction": "radical"},
        {"id": "victor_shadow", "name": "Victor Shadow", "faction": "opportunist"},
        {"id": "agent_nightingale", "name": "Nightingale", "faction": "neutral"}
    ]
    
    # 场景数据
    scenes = [
        {"id": "war_room", "name": "War Room", "type": "war_room"},
        {"id": "parliament", "name": "Parliament", "type": "parliament"},
        {"id": "mansion", "name": "Private Mansion", "type": "mansion"},
        {"id": "underground_bunker", "name": "Underground Bunker", "type": "underground_bunker"},
        {"id": "office", "name": "Executive Office", "type": "office"},
        {"id": "conference", "name": "Conference Room", "type": "conference"}
    ]
    
    # UI图标
    icons = [
        {"id": "icon_resource", "name": "Resource Icon", "type": "resource"},
        {"id": "icon_faction", "name": "Faction Icon", "type": "faction"},
        {"id": "icon_action", "name": "Action Icon", "type": "action"},
        {"id": "icon_status", "name": "Status Icon", "type": "status"}
    ]
    
    # 音乐配置
    music_tracks = [
        {"id": "bgm_title", "name": "Title Screen", "style": "dramatic", "mood": "tense", "duration": 120},
        {"id": "bgm_gameplay", "name": "Gameplay", "style": "ambient", "mood": "neutral", "duration": 300},
        {"id": "bgm_crisis", "name": "Crisis", "style": "tension", "mood": "urgent", "duration": 180},
        {"id": "bgm_victory", "name": "Victory", "style": "triumphant", "mood": "triumphant", "duration": 120}
    ]
    
    print("=" * 60)
    print("Power Game 资源生成器")
    print("=" * 60)
    print(f"\n设计系统: 主权情报设计系统")
    print(f"输出目录: {generator.output_dir}")
    print("\n" + "-" * 60)
    
    # 生成角色肖像
    print("\n[1/4] 生成角色肖像...")
    for char in characters:
        generator.generate_character_portrait(char)
        time.sleep(2)
    
    # 生成场景
    print("\n[2/4] 生成场景背景...")
    for scene in scenes:
        generator.generate_scene(scene)
        time.sleep(2)
    
    # 生成UI图标
    print("\n[3/4] 生成UI图标...")
    for icon in icons:
        generator.generate_ui_icon(icon)
        time.sleep(1)
    
    # 生成音乐配置
    print("\n[4/4] 生成音乐配置...")
    for music in music_tracks:
        generator.generate_music(music)
    
    print("\n" + "=" * 60)
    print("资源生成完成！")
    print("=" * 60)


if __name__ == "__main__":
    main()
