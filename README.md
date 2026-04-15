# Power Game: The Art of Political Intrigue

## Project Overview

Power Game is a deep strategy game centered on political power struggle. Players navigate complex political landscapes, managing resources, building factions, gathering intelligence, forming alliances, and making critical decisions that shape the nation's destiny.

### Game Features

- **Immersive Political Simulation**: Experience real power dynamics
- **Complex Character System**: 22 unique NPCs with political stances and hidden motives
- **Multi-path Narrative**: 8 core story nodes with branching choices
- **Strategic Depth**: Multi-layer decision trees with delayed consequences
- **8 Different Endings**: From dictatorship to democratic reform, shadow control to exile

## Core Systems

### Resource Management
- 8 Resource Types: Political Capital, Influence, Information, Wealth, Military, Reputation, Legitimacy, Public Support
- Resource Conversion with efficiency curves
- Delayed effects simulating political lag

### Faction & Loyalty System
- 5 Political Factions: Conservative Alliance, Liberal Coalition, Central Bloc, Radical Front, Opportunist Circle
- Loyalty, Trust, and Fear tracking for each faction member
- AI Decision Engine with behavior trees and utility calculations

### Intelligence System
- Spy deployment and network management
- Counter-intelligence operations

### Alliance & Betrayal
- 8 Treaty types
- Dynamic trust evolution
- Reputation impact from betrayals

## Technical Stack

- **C++17** game engine
- **SDL2** for GUI
- **nlohmann_json** for serialization
- **CMake** build system

## Build Instructions

```bash
git clone https://github.com/Theater-ahyeon/power-games-simulator.git
cd power-games-simulator
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
./build/bin/PowerGame
```

## Design System

Follows "The Sovereign Intelligence" design system:
- Dark background (#111316)
- Cyan accent (#00daf3)
- Gold highlights (#e9c349)
- Zero rounded corners
- Sharp edges

## License

MIT License - See LICENSE file

## Tags

`political-strategy-game` `game-development` `decision-making-simulator` `political-intrigue` `multi-ending-game`

---

*The game of power begins with every choice.*
