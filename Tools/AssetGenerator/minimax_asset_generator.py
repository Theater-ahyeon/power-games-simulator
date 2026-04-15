"""
MiniMax API Asset Generator for Power Game
This script generates character portraits, scene backgrounds, and UI elements
using the MiniMax multimodal API.
"""

import os
import json
import base64
import requests
from pathlib import Path
from typing import Optional, Dict, List
import time

class MiniMaxAssetGenerator:
    """Generates game assets using MiniMax API"""
    
    def __init__(self, api_key: str, output_dir: str = "Content/Art"):
        self.api_key = api_key
        self.output_dir = Path(output_dir)
        self.base_url = "https://api.minimax.chat/v1"
        
        self.output_dir.mkdir(parents=True, exist_ok=True)
        (self.output_dir / "Characters").mkdir(exist_ok=True)
        (self.output_dir / "Scenes").mkdir(exist_ok=True)
        (self.output_dir / "UI").mkdir(exist_ok=True)
        
    def _make_request(self, endpoint: str, payload: dict) -> Optional[dict]:
        """Make API request to MiniMax"""
        headers = {
            "Authorization": f"Bearer {self.api_key}",
            "Content-Type": "application/json"
        }
        
        try:
            response = requests.post(
                f"{self.base_url}/{endpoint}",
                headers=headers,
                json=payload,
                timeout=60
            )
            response.raise_for_status()
            return response.json()
        except requests.exceptions.RequestException as e:
            print(f"API request failed: {e}")
            return None
            
    def generate_character_portrait(
        self,
        character_name: str,
        description: str,
        faction: str,
        style: str = "political thriller aesthetic"
    ) -> Optional[str]:
        """Generate a character portrait image"""
        
        prompt = f"""
        Character portrait for {character_name} from the {faction} faction.
        Style: {style}
        Description: {description}
        
        Requirements:
        - Dark, atmospheric lighting
        - Professional headshot composition
        - Serious, political expression
        - Minimal color palette with faction-appropriate accents
        - No background clutter, focus on the character
        - High contrast, cinematic quality
        """
        
        payload = {
            "model": "image-01",
            "prompt": prompt.strip(),
            "num_images": 1,
            "aspect_ratio": "1:1",
            "response_format": "base64"
        }
        
        result = self._make_request("images/generations", payload)
        
        if result and "data" in result and len(result["data"]) > 0:
            image_data = result["data"][0]["b64_json"]
            
            output_path = self.output_dir / "Characters" / f"{character_name}.png"
            with open(output_path, "wb") as f:
                f.write(base64.b64decode(image_data))
                
            print(f"Generated portrait: {output_path}")
            return str(output_path)
            
        return None
        
    def generate_scene_background(
        self,
        scene_name: str,
        description: str,
        time_of_day: str = "evening"
    ) -> Optional[str]:
        """Generate a scene background image"""
        
        prompt = f"""
        Political thriller scene: {scene_name}
        Time: {time_of_day}
        Description: {description}
        
        Requirements:
        - Dark, moody atmosphere
        - Architectural elements suggesting power and authority
        - War room or government building aesthetic
        - High-tech displays and monitoring screens
        - Subtle lighting with dramatic shadows
        - Cinematic composition
        """
        
        payload = {
            "model": "image-01",
            "prompt": prompt.strip(),
            "num_images": 1,
            "aspect_ratio": "16:9",
            "response_format": "base64"
        }
        
        result = self._make_request("images/generations", payload)
        
        if result and "data" in result and len(result["data"]) > 0:
            image_data = result["data"][0]["b64_json"]
            
            output_path = self.output_dir / "Scenes" / f"{scene_name}.png"
            with open(output_path, "wb") as f:
                f.write(base64.b64decode(image_data))
                
            print(f"Generated scene: {output_path}")
            return str(output_path)
            
        return None
        
    def generate_ui_icon(
        self,
        icon_name: str,
        description: str,
        icon_type: str = "flat"
    ) -> Optional[str]:
        """Generate a UI icon"""
        
        prompt = f"""
        Game UI icon: {icon_name}
        Type: {icon_type} style
        Description: {description}
        
        Requirements:
        - Minimalist design
        - Single color or limited palette
        - Clear silhouette readable at small sizes
        - Political/power theme elements
        - Transparent background
        - Sharp, clean edges
        """
        
        payload = {
            "model": "image-01",
            "prompt": prompt.strip(),
            "num_images": 1,
            "aspect_ratio": "1:1",
            "response_format": "base64"
        }
        
        result = self._make_request("images/generations", payload)
        
        if result and "data" in result and len(result["data"]) > 0:
            image_data = result["data"][0]["b64_json"]
            
            output_path = self.output_dir / "UI" / "Icons" / f"{icon_name}.png"
            output_path.parent.mkdir(exist_ok=True)
            with open(output_path, "wb") as f:
                f.write(base64.b64decode(image_data))
                
            print(f"Generated icon: {output_path}")
            return str(output_path)
            
        return None
        
    def batch_generate_characters(self, characters: List[Dict]) -> Dict[str, str]:
        """Batch generate character portraits"""
        results = {}
        
        for char in characters:
            success = self.generate_character_portrait(
                character_name=char["name"],
                description=char["description"],
                faction=char["faction"]
            )
            if success:
                results[char["name"]] = success
                
            time.sleep(2)
            
        return results
        
    def batch_generate_scenes(self, scenes: List[Dict]) -> Dict[str, str]:
        """Batch generate scene backgrounds"""
        results = {}
        
        for scene in scenes:
            success = self.generate_scene_background(
                scene_name=scene["name"],
                description=scene["description"],
                time_of_day=scene.get("time", "evening")
            )
            if success:
                results[scene["name"]] = success
                
            time.sleep(2)
            
        return results


def main():
    api_key = os.environ.get("MINIMAX_API_KEY", "")
    
    if not api_key:
        print("Error: MINIMAX_API_KEY not set")
        return
        
    generator = MiniMaxAssetGenerator(api_key)
    
    characters = [
        {
            "name": "Chen_Weiming",
            "faction": "Conservative Alliance",
            "description": "62-year-old male politician with silver hair, sharp features, wearing a dark suit. Cold, calculating expression. Traditional East Asian appearance."
        },
        {
            "name": "Sarah_Jenkins",
            "faction": "Liberal Coalition",
            "description": "45-year-old female politician with blonde hair in a professional updo, wearing elegant business attire. Intelligent, determined expression."
        },
        {
            "name": "Commander_Fire",
            "faction": "Radical Front",
            "description": "40-year-old revolutionary leader with short dark hair, wearing military-style jacket. Intense, passionate expression. Visible scar on face."
        },
        {
            "name": "Victor_Shadow",
            "faction": "Opportunist Circle",
            "description": "50-year-old mysterious figure in shadow. Only eyes visible through darkness. Ambiguous gender. Ominous presence."
        }
    ]
    
    scenes = [
        {
            "name": "war_room",
            "description": "High-tech command center with multiple screens showing political data, maps, and surveillance feeds. Dark atmosphere with cyan accent lighting.",
            "time": "night"
        },
        {
            "name": "parliament",
            "description": "Grand parliamentary chamber with rows of seats. Dramatic lighting through tall windows. Symbol of democratic power.",
            "time": "evening"
        },
        {
            "name": "underground_bunker",
            "description": "Underground secure meeting room. Concrete walls, minimal furniture, single table with map. Emergency lighting.",
            "time": "night"
        },
        {
            "name": "mansion_lobby",
            "description": "Luxurious private mansion entrance. Marble floors, grand staircase, expensive art. Wealth and power on display.",
            "time": "evening"
        }
    ]
    
    print("Generating character portraits...")
    char_results = generator.batch_generate_characters(characters)
    print(f"Generated {len(char_results)} character portraits")
    
    print("\nGenerating scene backgrounds...")
    scene_results = generator.batch_generate_scenes(scenes)
    print(f"Generated {len(scene_results)} scene backgrounds")
    
    print("\nAsset generation complete!")


if __name__ == "__main__":
    main()
