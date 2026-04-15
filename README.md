# Power Game: The Art of Political Intrigue

A deep strategy game centered on political power struggle, offering immersive political博弈 experience and highly strategic gameplay.

## Game Overview

Power Game is a political strategy simulator set in a fictional modern society. Players navigate complex political landscapes, managing resources, building factions, gathering intelligence, forming alliances, and making critical decisions that shape the nation's destiny. Every choice carries consequences that ripple through the political ecosystem.

## Core Systems

### 1. Resource Management System
- **8 Resource Types**: Political Capital, Influence, Information, Wealth, Military, Reputation, Legitimacy, Public Support
- **Resource Conversion**: Convert between resource types with efficiency curves
- **Delayed Effects**: Some resource changes manifest after several turns
- **Seasonal Modifiers**: Resources regenerate at different rates based on in-game seasons

### 2. Faction & Loyalty System
- **5 Political Factions**: Conservative Alliance, Liberal Coalition, Central Bloc, Radical Front, Opportunist Circle
- **Loyalty Management**: Track loyalty, trust, and fear for each faction member
- **Talent Recruitment**: Discover and recruit new characters with unique skills
- **Defection Risk**: Calculate probability of character betrayal based on multiple factors

### 3. Intelligence Collection System
- **Spy Deployment**: Send agents to infiltrate rival factions
- **Intel Networks**: Build and manage information-gathering networks
- **Information Analysis**: Analyze gathered intelligence for reliability and disinformation
- **Counter-Intelligence**: Security sweeps and counter-espionage operations

### 4. Alliance & Betrayal System
- **8 Treaty Types**: Military Pact, Economic Alliance, Political Coalition, Intelligence Sharing, etc.
- **Dynamic Trust**: Trust levels evolve based on actions and events
- **Betrayal Mechanics**: Execute betrayals with calculated consequences
- **Reputation Impact**: Betrayals affect standing with all factions

### 5. Character System
- **22+ Unique NPCs** with distinct political stances and motivations
- **5 Political Stances**: Conservative, Liberal, Neutral, Radical, Opportunist
- **8 Core Attributes**: Scheming, Charisma, Intelligence, Wealth, Military, Reputation, Integrity, Ambition
- **Hidden Motives**: Personal goals, secret relationships, and betrayal tendencies
- **AI Decision Engine**: Utility-based AI with behavior trees for NPC behavior

### 6. Story System
- **8 Key Story Nodes** with 3-5 branching choices each
- **Dynamic Story Generation**: Context-sensitive events based on game state
- **Decision History Tracking**: Previous choices affect available options
- **Historical References**: Events inspired by Watergate, palace coups, financial crises

### 7. Strategy & Decision System
- **Decision Trees**: Multi-layered decision trees with 3+ levels of impact chains
- **Delayed Consequences**: Some effects manifest turns after the decision
- **Decision Analysis**: Short-term gain, long-term impact, and risk evaluation
- **Probability-based Outcomes**: Results vary based on calculated probabilities

### 8. Multi-Ending System
- **8 Distinct Endings**: Dictatorship, Democratic Reform, Power Balance, Exile, Shadow Control, System Collapse, Foreign Intervention, Personal Ambition
- **15+ Ending Conditions**: Complex evaluation based on resources, factions, and decisions
- **Weighted Scoring**: Endings evaluated using weighted multi-factor analysis

## Game Flow

```
Start Game → Power Vacuum Event → [Choose Path]
    ├─ Coalition Building → Intelligence War → Economic Crisis → Foreign Interference
    ├─ Conservative Consolidation → Intelligence War → Economic Crisis → Foreign Interference
    └─ Exploit Chaos → Intelligence War → Economic Crisis → Foreign Interference
        ↓
    Constitutional Crisis → [Critical Choice]
        ├─ Emergency Powers → Dictatorship Path
        ├─ Constitutional Reform → Democracy Path
        ├─ Grand Compromise → Balance Path
        └─ Military Coup → Collapse Path
            ↓
    Final Gambit → Ending Evaluation → One of 8 Endings
```

## Technical Architecture

### Build Requirements
- **C++17** compatible compiler (MSVC 2019+, GCC 9+, Clang 10+)
- **CMake 3.20+**
- **nlohmann_json** library (for JSON serialization)

### Build Instructions

#### Windows (MSVC)
```bash
git clone https://github.com/your-org/power-games-simulator.git
cd power-games-simulator
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

#### Linux/macOS
```bash
git clone https://github.com/your-org/power-games-simulator.git
cd power-games-simulator
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Run
```bash
./build/bin/PowerGame          # Run game
./build/bin/PowerGameTests     # Run unit tests
```

### Python Tools
```bash
pip install numpy
python Tools/DataAnalyzer/data_analyzer.py
python Tools/AITrainer/ai_trainer.py
```

## Project Structure

```
PowerGame/
├── Source/PowerGame/
│   ├── Core/           # Game framework, event system, time management
│   ├── Resources/      # Resource management system
│   ├── Faction/        # Faction, loyalty, recruitment systems
│   ├── Intelligence/   # Spy networks, analysis, counter-intel
│   ├── Alliance/       # Treaties, trust, betrayal mechanics
│   ├── Character/      # NPC system, AI, relationships
│   ├── Story/          # Story nodes, dynamic generation
│   ├── Strategy/       # Decision trees, delayed consequences
│   ├── Ending/         # Multi-ending evaluation system
│   ├── UI/             # UI panels and rendering
│   ├── Data/           # Save/load, serialization
│   ├── AI/             # Behavior trees, utility AI
│   └── Game/           # Main application entry point
├── Data/               # JSON data files
│   ├── Characters/     # Character definitions
│   ├── Stories/        # Story node data
│   ├── Endings/        # Ending conditions
│   └── Config/         # Game configuration
├── Content/            # Game assets
├── Tests/              # Unit and integration tests
├── Tools/              # Python analysis tools
├── Docs/               # Documentation
└── CMakeLists.txt      # Build configuration
```

## Design Philosophy

Power Game draws inspiration from real-world political events and power dynamics:

| Historical Reference | Game Application |
|---------------------|------------------|
| Watergate Scandal | Intelligence War node - surveillance and cover-up |
| Palace Coup Dynamics | Power Vacuum - rapid power transfer logic |
| 2008 Financial Crisis | Economic Crisis - systemic failure decisions |
| Arab Spring | Radical faction - mass mobilization |
| Cold War Espionage | Intelligence system - spy networks |
| Color Revolutions | Democratic Reform path - foreign influence |
| Constitutional Crises | Legal framework challenges |
| Corporate Lobbying | Wealth-to-influence conversion |

## Version Roadmap

### v1.0 (Current)
- Core gameplay systems
- 22+ characters with AI
- 8 main story nodes
- 8 endings
- Basic UI framework

### v1.1 (Planned)
- Enhanced UI with graphical rendering
- Sound effects and music
- Tutorial system
- Achievement tracking
- Steam integration

### v1.2 (Planned)
- Multiplayer mode (diplomatic negotiations)
- Scenario editor
- Mod support
- Additional story arcs
- Extended ending variations

### v2.0 (Planned)
- Full 3D visualization
- Voice acting
- Advanced AI with machine learning
- Community workshop
- Cross-platform mobile support

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Tags

`political-strategy-game` `game-development` `decision-making-simulator` `political-intrigue` `multi-ending-game`
