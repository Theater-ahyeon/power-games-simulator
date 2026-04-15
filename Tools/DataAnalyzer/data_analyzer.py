"""
PowerGame Data Analyzer - Political Strategy Game Data Analysis Tool
Analyzes game state data, decision patterns, and outcome predictions.
"""

import json
import numpy as np
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple
from enum import Enum
from collections import defaultdict
import statistics


class ResourceType(Enum):
    POLITICAL_CAPITAL = "PoliticalCapital"
    INFLUENCE = "Influence"
    INFORMATION = "Information"
    WEALTH = "Wealth"
    MILITARY = "Military"
    REPUTATION = "Reputation"
    LEGITIMACY = "Legitimacy"
    PUBLIC_SUPPORT = "PublicSupport"


class PoliticalStance(Enum):
    CONSERVATIVE = "Conservative"
    LIBERAL = "Liberal"
    NEUTRAL = "Neutral"
    RADICAL = "Radical"
    OPPORTUNIST = "Opportunist"


class EndingType(Enum):
    DICTATORSHIP = "Dictatorship"
    DEMOCRATIC_REFORM = "DemocraticReform"
    POWER_BALANCE = "PowerBalance"
    EXILE_FAILURE = "ExileFailure"
    SHADOW_CONTROL = "ShadowControl"
    SYSTEM_COLLAPSE = "SystemCollapse"
    FOREIGN_INTERVENTION = "ForeignIntervention"
    PERSONAL_AMBITION = "PersonalAmbition"


@dataclass
class CharacterData:
    id: str
    name: str
    stance: PoliticalStance
    attributes: Dict[str, float]
    hidden_motive: Dict
    faction_id: str
    loyalty: float = 50.0
    health: float = 100.0


@dataclass
class GameState:
    turn: int
    resources: Dict[str, float]
    faction_powers: Dict[str, float]
    characters: List[CharacterData]
    decisions: List[Dict]
    trust_matrix: Dict[str, float]
    betrayals: List[Dict]


class PoliticalAnalyzer:
    def __init__(self):
        self.history: List[GameState] = []
        self.decision_outcomes: Dict[str, List[float]] = defaultdict(list)

    def add_state(self, state: GameState):
        self.history.append(state)

    def calculate_political_tension(self, state: GameState) -> float:
        powers = list(state.faction_powers.values())
        if not powers:
            return 0.0
        total = sum(powers)
        if total == 0:
            return 0.0
        max_power = max(powers)
        dominance = max_power / total
        balance = 1.0 - dominance
        num_factions = len(powers)
        if num_factions > 1:
            variance = np.var(powers)
            balance_score = 1.0 - min(variance / 2500.0, 1.0)
        else:
            balance_score = 0.0
        tension = (1.0 - balance) * 60.0 + (1.0 - balance_score) * 40.0
        return min(max(tension, 0.0), 100.0)

    def calculate_power_index(self, character: CharacterData) -> float:
        attrs = character.attributes
        return (attrs.get('scheming', 0) * 0.25 +
                attrs.get('wealth', 0) * 0.20 +
                attrs.get('military', 0) * 0.15 +
                attrs.get('charisma', 0) * 0.15 +
                attrs.get('intelligence', 0) * 0.10 +
                attrs.get('reputation', 0) * 0.10 +
                attrs.get('ambition', 0) * 0.05)

    def calculate_trust_index(self, character: CharacterData) -> float:
        attrs = character.attributes
        return (attrs.get('integrity', 0) * 0.40 +
                (100.0 - attrs.get('ambition', 0)) * 0.30 +
                (100.0 - attrs.get('scheming', 0)) * 0.30)

    def predict_betrayal_risk(self, character: CharacterData, target_id: str,
                               trust_level: float = 50.0) -> float:
        motive = character.hidden_motive
        betrayal_tendency = motive.get('betrayalTendency', 0)
        ambition = character.attributes.get('ambition', 50)
        risk = (betrayal_tendency * 0.40 +
                ambition * 0.25 +
                (100.0 - trust_level) * 0.20 +
                (100.0 - character.loyalty) * 0.15)
        return min(max(risk, 0.0), 100.0)

    def evaluate_ending_probability(self, state: GameState) -> Dict[EndingType, float]:
        scores = {}
        resources = state.resources

        scores[EndingType.DICTATORSHIP] = (
            resources.get('PoliticalCapital', 0) * 0.003 +
            resources.get('Military', 0) * 0.003 +
            (100.0 - resources.get('Legitimacy', 50)) * 0.002 +
            state.faction_powers.get('conservatives', 0) * 0.002
        )

        scores[EndingType.DEMOCRATIC_REFORM] = (
            resources.get('Legitimacy', 0) * 0.003 +
            resources.get('PublicSupport', 0) * 0.003 +
            resources.get('Reputation', 0) * 0.002 +
            state.faction_powers.get('liberals', 0) * 0.002
        )

        scores[EndingType.POWER_BALANCE] = (
            resources.get('Influence', 0) * 0.003 +
            (100.0 - self.calculate_political_tension(state)) * 0.003 +
            state.faction_powers.get('neutrals', 0) * 0.002
        )

        scores[EndingType.EXILE_FAILURE] = (
            (100.0 - resources.get('PoliticalCapital', 50)) * 0.003 +
            (100.0 - resources.get('Reputation', 50)) * 0.003 +
            (100.0 - resources.get('Legitimacy', 50)) * 0.002
        )

        scores[EndingType.SHADOW_CONTROL] = (
            resources.get('Information', 0) * 0.003 +
            resources.get('Influence', 0) * 0.002 +
            state.faction_powers.get('opportunists', 0) * 0.002
        )

        scores[EndingType.SYSTEM_COLLAPSE] = (
            (100.0 - resources.get('Legitimacy', 50)) * 0.003 +
            (100.0 - resources.get('PublicSupport', 50)) * 0.003 +
            self.calculate_political_tension(state) * 0.002
        )

        scores[EndingType.FOREIGN_INTERVENTION] = (
            (100.0 - resources.get('Military', 50)) * 0.003 +
            (100.0 - resources.get('Legitimacy', 50)) * 0.002 +
            (100.0 - resources.get('Wealth', 50)) * 0.002
        )

        scores[EndingType.PERSONAL_AMBITION] = (
            resources.get('Wealth', 0) * 0.003 +
            resources.get('Information', 0) * 0.002 +
            resources.get('PoliticalCapital', 0) * 0.002 +
            state.faction_powers.get('opportunists', 0) * 0.001
        )

        total = sum(scores.values())
        if total > 0:
            for key in scores:
                scores[key] = scores[key] / total * 100.0

        return scores

    def analyze_resource_trends(self) -> Dict[str, Dict[str, float]]:
        if len(self.history) < 2:
            return {}
        trends = {}
        for res_type in ResourceType:
            values = [s.resources.get(res_type.value, 0) for s in self.history]
            if len(values) >= 2:
                trend = values[-1] - values[0]
                avg = statistics.mean(values)
                std = statistics.stdev(values) if len(values) > 1 else 0
                trends[res_type.value] = {
                    'current': values[-1],
                    'average': avg,
                    'std_dev': std,
                    'trend': trend,
                    'trend_direction': 'up' if trend > 0 else 'down' if trend < 0 else 'stable'
                }
        return trends

    def find_optimal_strategy(self, state: GameState) -> Dict[str, float]:
        ending_probs = self.evaluate_ending_probability(state)
        dominant_ending = max(ending_probs, key=ending_probs.get)
        recommendations = {}

        if dominant_ending == EndingType.DICTATORSHIP:
            recommendations = {'Military': 0.8, 'PoliticalCapital': 0.7, 'Legitimacy': -0.3}
        elif dominant_ending == EndingType.DEMOCRATIC_REFORM:
            recommendations = {'PublicSupport': 0.9, 'Legitimacy': 0.8, 'Reputation': 0.7}
        elif dominant_ending == EndingType.POWER_BALANCE:
            recommendations = {'Influence': 0.8, 'Reputation': 0.6, 'Information': 0.5}
        elif dominant_ending == EndingType.SHADOW_CONTROL:
            recommendations = {'Information': 0.9, 'Influence': 0.7, 'Wealth': 0.5}
        elif dominant_ending == EndingType.EXILE_FAILURE:
            recommendations = {'PoliticalCapital': 0.9, 'Reputation': 0.8, 'Military': 0.7}
        elif dominant_ending == EndingType.SYSTEM_COLLAPSE:
            recommendations = {'Legitimacy': 0.9, 'PublicSupport': 0.8, 'Wealth': 0.6}
        else:
            recommendations = {'Wealth': 0.7, 'Information': 0.6, 'Influence': 0.5}

        return recommendations

    def generate_report(self, state: GameState) -> Dict:
        return {
            'turn': state.turn,
            'political_tension': self.calculate_political_tension(state),
            'ending_probabilities': {e.value: p for e, p in self.evaluate_ending_probability(state).items()},
            'resource_trends': self.analyze_resource_trends(),
            'optimal_strategy': self.find_optimal_strategy(state),
            'faction_balance': self._calculate_faction_balance(state),
            'key_risks': self._identify_key_risks(state)
        }

    def _calculate_faction_balance(self, state: GameState) -> float:
        powers = list(state.faction_powers.values())
        if not powers:
            return 1.0
        total = sum(powers)
        if total == 0:
            return 1.0
        max_power = max(powers)
        return 1.0 - (max_power / total)

    def _identify_key_risks(self, state: GameState) -> List[Dict]:
        risks = []
        if state.resources.get('Legitimacy', 50) < 30:
            risks.append({'type': 'legitimacy_crisis', 'severity': 'high',
                         'description': 'Legitimacy critically low - regime stability at risk'})
        if state.resources.get('PublicSupport', 50) < 25:
            risks.append({'type': 'public_unrest', 'severity': 'high',
                         'description': 'Public support dangerously low - unrest likely'})
        if self.calculate_political_tension(state) > 70:
            risks.append({'type': 'political_crisis', 'severity': 'medium',
                         'description': 'High political tension - crisis may erupt'})
        if state.resources.get('Military', 50) < 20:
            risks.append({'type': 'security_vulnerability', 'severity': 'medium',
                         'description': 'Military strength insufficient for security'})
        return risks


def analyze_save_file(filepath: str) -> Dict:
    with open(filepath, 'r') as f:
        data = json.load(f)
    analyzer = PoliticalAnalyzer()
    state = GameState(
        turn=data.get('turn', 0),
        resources=data.get('resources', {}),
        faction_powers=data.get('faction_powers', {}),
        characters=[],
        decisions=data.get('decisions', []),
        trust_matrix=data.get('trust_matrix', {}),
        betrayals=data.get('betrayals', [])
    )
    return analyzer.generate_report(state)


if __name__ == '__main__':
    analyzer = PoliticalAnalyzer()
    test_state = GameState(
        turn=10,
        resources={'PoliticalCapital': 55, 'Influence': 40, 'Information': 35,
                   'Wealth': 60, 'Military': 25, 'Reputation': 45,
                   'Legitimacy': 50, 'PublicSupport': 40},
        faction_powers={'conservatives': 35, 'liberals': 25, 'neutrals': 20,
                       'radicals': 15, 'opportunists': 10},
        characters=[],
        decisions=[],
        trust_matrix={},
        betrayals=[]
    )
    report = analyzer.generate_report(test_state)
    print(json.dumps(report, indent=2, default=str))
