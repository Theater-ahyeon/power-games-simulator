# Power Game - Game Design Document (GDD)

## 1. Game Vision

### 1.1 Concept Statement
Power Game is a deep political strategy simulator that puts players at the center of a complex power struggle in a fictional modern nation. Through resource management, faction manipulation, intelligence operations, and critical decision-making, players experience the thrill and consequences of political intrigue.

### 1.2 Target Audience
- Strategy game enthusiasts (ages 16+)
- Political simulation fans
- Narrative-driven game players
- Deep strategy gamers who enjoy Crusader Kings, Civilization, and Disco Elysium

### 1.3 Core Experience
The player should feel the weight of every decision, the tension of uncertain alliances, and the satisfaction of outmaneuvering political opponents through superior strategy and information control.

## 2. Gameplay Systems Design

### 2.1 Resource Management

#### Resource Types and Interactions
| Resource | Description | Regen Rate | Key Interactions |
|----------|-------------|------------|------------------|
| Political Capital | Direct political power and influence | 2.0/turn | Primary currency for actions |
| Influence | Ability to sway opinions and events | 1.5/turn | Converts to Reputation |
| Information | Intelligence and knowledge | 1.0/turn | Enables informed decisions |
| Wealth | Economic resources | 2.0/turn | Converts to most resources |
| Military | Armed force capability | 0.5/turn | Enables forceful actions |
| Reputation | Public standing | 0.0/turn | Gained through actions |
| Legitimacy | Right to rule | 0.0/turn | Critical for stability |
| Public Support | Popular backing | 1.0/turn | Enables democratic path |

#### Resource Conversion Matrix
```
Wealth → Political Capital (80% efficiency)
Information → Influence (90% efficiency)
Political Capital → Influence (85% efficiency)
Wealth → Information (70% efficiency)
Influence → Reputation (75% efficiency)
Political Capital → Legitimacy (60% efficiency)
Wealth → Military (65% efficiency)
Public Support → Legitimacy (80% efficiency)
```

### 2.2 Faction System

#### Faction Profiles
1. **Conservative Alliance**: Traditional power holders, strong military connections, resistant to change
2. **Liberal Coalition**: Reform advocates, strong public support, vulnerable to political attacks
3. **Central Bloc**: Pragmatic mediators, balanced resources, key swing faction
4. **Radical Front**: Revolutionary elements, high ambition, unpredictable behavior
5. **Opportunist Circle**: Self-interested operators, strong intelligence networks, unreliable allies

#### Loyalty Mechanics
- **Loyalty Score** (0-100): Primary commitment indicator
- **Trust Score** (0-100): Reliability assessment
- **Fear Factor** (0-100): Coercion-based compliance
- **Defection Risk** = (100-Loyalty)*0.4 + (100-Trust)*0.3 + Ambition*0.3

### 2.3 Intelligence System

#### Spy Operations
- **Deployment Cost**: 10-30 Wealth per agent
- **Gather Chance**: Skill*0.5 + EmbeddedBonus - RiskPenalty
- **Exposure Rate**: +0.5% per turn + turn*0.01%
- **Detection Chance**: Security*0.4 + Exposure*0.4 - EmbeddedBonus

#### Intelligence Types
1. Political Secrets - Faction internal dynamics
2. Financial Records - Economic leverage material
3. Military Intel - Force disposition and plans
4. Personal Scandals - Character vulnerability data
5. Alliance Details - Treaty terms and secret clauses
6. Betrayal Plans - Imminent treachery warnings
7. Policy Leaks - Upcoming policy changes
8. Foreign Connections - External influence evidence

### 2.4 Alliance & Betrayal System

#### Treaty Types and Effects
| Treaty Type | Trust Boost | Stability | Duration |
|-------------|-------------|-----------|----------|
| Military Pact | +15 | High | 20 turns |
| Economic Alliance | +10 | Medium | 15 turns |
| Political Coalition | +20 | High | 25 turns |
| Intelligence Sharing | +5 | Low | 10 turns |
| Non-Aggression Pact | +5 | Medium | 15 turns |
| Secret Treaty | +25 | Very Low | 5 turns |
| Marriage Alliance | +30 | Very High | Permanent |
| Trade Agreement | +8 | Medium | 10 turns |

#### Betrayal Consequence Scale
- **Minor (1-30)**: Trust reduction, private warning
- **Moderate (31-60)**: Formal complaint, treaty review, economic sanctions
- **Major (61-80)**: Alliance dissolution, diplomatic protest, reputation damage
- **Severe (81-100)**: Full diplomatic rupture, possible military response

### 2.5 Character System

#### Character Design Template
Each character has:
- **Political Stance**: Determines baseline behavior patterns
- **8 Attributes**: Define capabilities and tendencies
- **Hidden Motive**: Personal agenda unknown to player initially
- **Relationship Network**: Dynamic connections with other characters
- **AI Decision Profile**: Utility function weights for AI behavior

#### Character Balance
- **Conservative characters**: High Wealth/Military, low flexibility
- **Liberal characters**: High Reputation/Integrity, low Military
- **Neutral characters**: Balanced attributes, moderate in all areas
- **Radical characters**: High Ambition/Scheming, low Integrity
- **Opportunist characters**: High Scheming/Intelligence, very low Integrity

### 2.6 Story Architecture

#### Main Storyline Flow
```
Turn 1:   The Power Vacuum (5 choices)
Turn 5:   Coalition Building / Conservative Consolidation (3 choices each)
Turn 15:  The Intelligence War (4 choices)
Turn 25:  The Economic Crisis (4 choices)
Turn 35:  The Foreign Interference (4 choices)
Turn 50:  The Constitutional Crisis (4 choices)
Turn 70:  The Final Gambit (4 choices)
Turn 90:  The New Order (2 choices)
```

#### Dynamic Event Generation
- **Trigger Conditions**: Political tension, resource levels, faction balance
- **Frequency**: 1-3 dynamic events per 5 turns
- **Types**: Crisis events, opportunity events, character events, faction events
- **Scaling**: Difficulty and impact scale with game progression

### 2.7 Strategy & Decision System

#### Decision Evaluation Framework
```
Overall Score = ShortTermGain * 0.3 + LongTermImpact * 0.4 +
                (1 - Risk/100) * 0.2 + MoralWeight * 0.1
```

#### Decision Tree Depth
- **Level 1**: Immediate choice (current turn)
- **Level 2**: Secondary effects (3-5 turns later)
- **Level 3**: Long-term consequences (10-20 turns later)
- **Level 4+**: Endgame implications (affects ending determination)

### 2.8 Multi-Ending System

#### Ending Determination Formula
```
Ending Score = ResourceMatch * 0.35 + FactionMatch * 0.25 +
               DecisionMatch * 0.40 * (Priority/100)
```

#### Ending Descriptions
1. **The Iron Throne (Dictatorship)**: Absolute power through force and suppression
2. **The Dawn of Democracy (Democratic Reform)**: Peaceful transition to democracy
3. **The Grand Equilibrium (Power Balance)**: Stable multi-faction balance
4. **The Fallen Leader (Exile/Failure)**: Political downfall and exile
5. **The Puppet Master (Shadow Control)**: Behind-the-scenes manipulation
6. **The House of Cards (System Collapse)**: Total political breakdown
7. **The External Hand (Foreign Intervention)**: Foreign domination
8. **The Self-Made Sovereign (Personal Ambition)**: Personal empire achieved

## 3. AI System Design

### 3.1 NPC Decision Architecture
- **Utility AI**: Score-based action selection
- **Behavior Trees**: Complex behavior sequencing
- **Stance Bias**: Political stance modifies utility calculations
- **Context Sensitivity**: Decisions adapt to current game state

### 3.2 AI Training Pipeline
1. Generate synthetic training data from game simulations
2. Train decision model using gradient descent
3. Validate against held-out test data
4. Export trained weights for C++ integration

## 4. Technical Specifications

### 4.1 Performance Targets
- **Frame Rate**: 60 FPS stable on mid-range PC
- **Memory**: < 4GB RAM
- **Load Time**: < 5 seconds for game start
- **Save/Load**: < 2 seconds

### 4.2 Platform Support
- **Primary**: Windows 10/11 (x64)
- **Secondary**: macOS 12+, Linux (Ubuntu 22.04+)
- **Future**: iOS/Android (v2.0)

### 4.3 Data Persistence
- Save game format: Custom binary with text header
- Auto-save every 5 turns
- Maximum 100 save slots
- Cloud save support (v1.1)

## 5. Historical References

| Real-World Event | Game Mechanic | Story Node |
|------------------|---------------|------------|
| Watergate Scandal (1972) | Intelligence gathering, cover-up mechanics | The Intelligence War |
| Palace Coup Patterns | Rapid power transfer, loyalty shifts | The Power Vacuum |
| 2008 Financial Crisis | Economic policy decisions, bailout choices | The Economic Crisis |
| Arab Spring (2010-2012) | Mass mobilization, regime change | Radical faction events |
| Cold War Espionage | Spy networks, counter-intelligence | Intelligence system |
| Color Revolutions | Foreign influence, democratic movements | Foreign Interference |
| UK Constitutional Crises | Legal framework challenges | Constitutional Crisis |
| Corporate Lobbying (US) | Wealth-to-influence conversion | Opportunist mechanics |
