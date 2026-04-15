"""
PowerGame AI Trainer - Decision Model Training and Evaluation
Trains AI decision models for NPC behavior using game simulation data.
"""

import json
import numpy as np
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple
from enum import Enum
import random
import math


class DecisionType(Enum):
    SUPPORT = "Support"
    OPPOSE = "Oppose"
    NEUTRALITY = "Neutrality"
    BETRAYAL = "Betrayal"
    ALLIANCE = "Alliance"
    MANIPULATION = "Manipulation"
    SELF_PRESERVATION = "SelfPreservation"


@dataclass
class TrainingSample:
    character_id: str
    stance: str
    attributes: Dict[str, float]
    context: Dict[str, float]
    decision: DecisionType
    outcome_score: float
    turn: int


@dataclass
class WeightSet:
    scheming_weight: float = 0.25
    charisma_weight: float = 0.10
    intelligence_weight: float = 0.10
    wealth_weight: float = 0.15
    military_weight: float = 0.10
    reputation_weight: float = 0.05
    integrity_weight: float = 0.15
    ambition_weight: float = 0.10


class DecisionModel:
    def __init__(self, weights: Optional[WeightSet] = None):
        self.weights = weights or WeightSet()
        self.decision_thresholds = {
            DecisionType.SUPPORT: 0.6,
            DecisionType.OPPOSE: 0.5,
            DecisionType.NEUTRALITY: 0.4,
            DecisionType.BETRAYAL: 0.7,
            DecisionType.ALLIANCE: 0.55,
            DecisionType.MANIPULATION: 0.65,
            DecisionType.SELF_PRESERVATION: 0.5,
        }
        self.stance_biases = {
            'Conservative': {DecisionType.SUPPORT: 0.15, DecisionType.OPPOSE: -0.05,
                           DecisionType.BETRAYAL: -0.10, DecisionType.SELF_PRESERVATION: 0.10},
            'Liberal': {DecisionType.OPPOSE: 0.10, DecisionType.ALLIANCE: 0.10,
                       DecisionType.BETRAYAL: -0.15, DecisionType.MANIPULATION: -0.05},
            'Neutral': {DecisionType.NEUTRALITY: 0.20, DecisionType.ALLIANCE: 0.05,
                       DecisionType.BETRAYAL: -0.10},
            'Radical': {DecisionType.OPPOSE: 0.15, DecisionType.BETRAYAL: 0.10,
                       DecisionType.MANIPULATION: 0.05, DecisionType.SUPPORT: -0.10},
            'Opportunist': {DecisionType.MANIPULATION: 0.15, DecisionType.BETRAYAL: 0.10,
                          DecisionType.SUPPORT: -0.05, DecisionType.SELF_PRESERVATION: 0.10},
        }

    def calculate_utility(self, attributes: Dict[str, float], decision: DecisionType,
                          stance: str, context: Dict[str, float]) -> float:
        w = self.weights
        base_utility = (attributes.get('scheming', 50) * w.scheming_weight +
                       attributes.get('charisma', 50) * w.charisma_weight +
                       attributes.get('intelligence', 50) * w.intelligence_weight +
                       attributes.get('wealth', 50) * w.wealth_weight +
                       attributes.get('military', 50) * w.military_weight +
                       attributes.get('reputation', 50) * w.reputation_weight +
                       attributes.get('integrity', 50) * w.integrity_weight +
                       attributes.get('ambition', 50) * w.ambition_weight)

        stance_bias = self.stance_biases.get(stance, {}).get(decision, 0.0)
        threshold = self.decision_thresholds.get(decision, 0.5)

        decision_modifier = self._get_decision_modifier(decision, attributes)
        context_modifier = self._get_context_modifier(decision, context)

        utility = (base_utility / 100.0 + stance_bias + decision_modifier + context_modifier)
        return max(0.0, min(1.0, utility))

    def _get_decision_modifier(self, decision: DecisionType, attributes: Dict[str, float]) -> float:
        modifiers = {
            DecisionType.SUPPORT: attributes.get('integrity', 50) * 0.002 - 0.05,
            DecisionType.OPPOSE: attributes.get('ambition', 50) * 0.002 - 0.05,
            DecisionType.NEUTRALITY: (100 - attributes.get('ambition', 50)) * 0.001,
            DecisionType.BETRAYAL: (attributes.get('scheming', 50) * 0.003 +
                                    attributes.get('ambition', 50) * 0.002 -
                                    attributes.get('integrity', 50) * 0.003),
            DecisionType.ALLIANCE: attributes.get('charisma', 50) * 0.002,
            DecisionType.MANIPULATION: (attributes.get('scheming', 50) * 0.003 -
                                       attributes.get('integrity', 50) * 0.001),
            DecisionType.SELF_PRESERVATION: 0.0,
        }
        return modifiers.get(decision, 0.0)

    def _get_context_modifier(self, decision: DecisionType, context: Dict[str, float]) -> float:
        tension = context.get('political_tension', 50)
        threat = context.get('personal_threat', 30)

        modifiers = {
            DecisionType.SUPPORT: -tension * 0.001,
            DecisionType.OPPOSE: tension * 0.002,
            DecisionType.NEUTRALITY: 0.0,
            DecisionType.BETRAYAL: tension * 0.001 + threat * 0.002,
            DecisionType.ALLIANCE: tension * 0.001,
            DecisionType.MANIPULATION: tension * 0.001,
            DecisionType.SELF_PRESERVATION: threat * 0.003,
        }
        return modifiers.get(decision, 0.0)

    def decide(self, attributes: Dict[str, float], stance: str,
               context: Dict[str, float]) -> Tuple[DecisionType, float]:
        best_decision = DecisionType.NEUTRALITY
        best_utility = -1.0

        for decision in DecisionType:
            utility = self.calculate_utility(attributes, decision, stance, context)
            if utility > best_utility:
                best_utility = utility
                best_decision = decision

        return best_decision, best_utility


class AITrainer:
    def __init__(self, model: Optional[DecisionModel] = None):
        self.model = model or DecisionModel()
        self.training_data: List[TrainingSample] = []
        self.validation_data: List[TrainingSample] = []
        self.training_history: List[Dict] = []

    def add_training_sample(self, sample: TrainingSample):
        self.training_data.append(sample)

    def load_training_data(self, filepath: str):
        with open(filepath, 'r') as f:
            data = json.load(f)
        for item in data:
            sample = TrainingSample(
                character_id=item['character_id'],
                stance=item['stance'],
                attributes=item['attributes'],
                context=item['context'],
                decision=DecisionType(item['decision']),
                outcome_score=item['outcome_score'],
                turn=item['turn']
            )
            self.training_data.append(sample)

    def split_data(self, validation_ratio: float = 0.2):
        random.shuffle(self.training_data)
        split_idx = int(len(self.training_data) * (1 - validation_ratio))
        self.validation_data = self.training_data[split_idx:]
        self.training_data = self.training_data[:split_idx]

    def train(self, epochs: int = 100, learning_rate: float = 0.01) -> List[Dict]:
        history = []
        for epoch in range(epochs):
            total_loss = 0.0
            correct = 0

            for sample in self.training_data:
                predicted_decision, predicted_utility = self.model.decide(
                    sample.attributes, sample.stance, sample.context)

                if predicted_decision == sample.decision:
                    correct += 1

                target_utility = sample.outcome_score / 100.0
                loss = (predicted_utility - target_utility) ** 2
                total_loss += loss

                self._update_weights(sample, predicted_utility, target_utility, learning_rate)

            accuracy = correct / max(len(self.training_data), 1)
            avg_loss = total_loss / max(len(self.training_data), 1)

            val_accuracy = self._evaluate_validation()

            epoch_data = {
                'epoch': epoch + 1,
                'loss': avg_loss,
                'accuracy': accuracy,
                'validation_accuracy': val_accuracy
            }
            history.append(epoch_data)
            self.training_history.append(epoch_data)

        return history

    def _update_weights(self, sample: TrainingSample, predicted: float,
                        target: float, learning_rate: float):
        error = predicted - target
        gradient = error * learning_rate * 0.001

        w = self.model.weights
        w.scheming_weight = max(0.01, w.scheming_weight - gradient * sample.attributes.get('scheming', 50))
        w.charisma_weight = max(0.01, w.charisma_weight - gradient * sample.attributes.get('charisma', 50))
        w.intelligence_weight = max(0.01, w.intelligence_weight - gradient * sample.attributes.get('intelligence', 50))
        w.wealth_weight = max(0.01, w.wealth_weight - gradient * sample.attributes.get('wealth', 50))
        w.military_weight = max(0.01, w.military_weight - gradient * sample.attributes.get('military', 50))
        w.reputation_weight = max(0.01, w.reputation_weight - gradient * sample.attributes.get('reputation', 50))
        w.integrity_weight = max(0.01, w.integrity_weight - gradient * sample.attributes.get('integrity', 50))
        w.ambition_weight = max(0.01, w.ambition_weight - gradient * sample.attributes.get('ambition', 50))

        total = (w.scheming_weight + w.charisma_weight + w.intelligence_weight +
                w.wealth_weight + w.military_weight + w.reputation_weight +
                w.integrity_weight + w.ambition_weight)
        w.scheming_weight /= total
        w.charisma_weight /= total
        w.intelligence_weight /= total
        w.wealth_weight /= total
        w.military_weight /= total
        w.reputation_weight /= total
        w.integrity_weight /= total
        w.ambition_weight /= total

    def _evaluate_validation(self) -> float:
        if not self.validation_data:
            return 0.0
        correct = 0
        for sample in self.validation_data:
            predicted, _ = self.model.decide(sample.attributes, sample.stance, sample.context)
            if predicted == sample.decision:
                correct += 1
        return correct / len(self.validation_data)

    def evaluate(self, test_data: List[TrainingSample]) -> Dict:
        correct = 0
        total_loss = 0.0
        decision_counts = {d: 0 for d in DecisionType}
        correct_counts = {d: 0 for d in DecisionType}

        for sample in test_data:
            predicted, utility = self.model.decide(sample.attributes, sample.stance, sample.context)
            decision_counts[predicted] += 1
            if predicted == sample.decision:
                correct += 1
                correct_counts[predicted] += 1
            total_loss += (utility - sample.outcome_score / 100.0) ** 2

        accuracy = correct / max(len(test_data), 1)
        avg_loss = total_loss / max(len(test_data), 1)

        per_decision_accuracy = {}
        for d in DecisionType:
            if decision_counts[d] > 0:
                per_decision_accuracy[d.value] = correct_counts[d] / decision_counts[d]

        return {
            'accuracy': accuracy,
            'average_loss': avg_loss,
            'per_decision_accuracy': per_decision_accuracy,
            'decision_distribution': {d.value: c for d, c in decision_counts.items()},
            'total_samples': len(test_data)
        }

    def export_model(self, filepath: str):
        model_data = {
            'weights': {
                'scheming': self.model.weights.scheming_weight,
                'charisma': self.model.weights.charisma_weight,
                'intelligence': self.model.weights.intelligence_weight,
                'wealth': self.model.weights.wealth_weight,
                'military': self.model.weights.military_weight,
                'reputation': self.model.weights.reputation_weight,
                'integrity': self.model.weights.integrity_weight,
                'ambition': self.model.weights.ambition_weight,
            },
            'thresholds': {d.value: t for d, t in self.model.decision_thresholds.items()},
            'stance_biases': {
                stance: {d.value: b for d, b in biases.items()}
                for stance, biases in self.model.stance_biases.items()
            },
            'training_history': self.training_history[-10:]
        }
        with open(filepath, 'w') as f:
            json.dump(model_data, f, indent=2)


def generate_synthetic_training_data(num_samples: int = 1000) -> List[TrainingSample]:
    stances = ['Conservative', 'Liberal', 'Neutral', 'Radical', 'Opportunist']
    samples = []

    for i in range(num_samples):
        stance = random.choice(stances)
        attributes = {
            'scheming': random.uniform(20, 95),
            'charisma': random.uniform(20, 95),
            'intelligence': random.uniform(20, 95),
            'wealth': random.uniform(20, 95),
            'military': random.uniform(20, 95),
            'reputation': random.uniform(20, 95),
            'integrity': random.uniform(20, 95),
            'ambition': random.uniform(20, 95),
        }
        context = {
            'political_tension': random.uniform(10, 90),
            'personal_threat': random.uniform(5, 70),
            'resource_level': random.uniform(20, 80),
        }

        if stance == 'Conservative':
            if context['political_tension'] > 60:
                decision = DecisionType.OPPOSE
            else:
                decision = DecisionType.SUPPORT
        elif stance == 'Liberal':
            if context['political_tension'] > 40:
                decision = DecisionType.OPPOSE
            else:
                decision = DecisionType.ALLIANCE
        elif stance == 'Neutral':
            decision = DecisionType.NEUTRALITY
        elif stance == 'Radical':
            if context['political_tension'] > 30:
                decision = DecisionType.OPPOSE
            else:
                decision = DecisionType.MANIPULATION
        else:
            if context['personal_threat'] > 50:
                decision = DecisionType.SELF_PRESERVATION
            else:
                decision = DecisionType.MANIPULATION

        outcome_score = random.uniform(30, 90)
        samples.append(TrainingSample(
            character_id=f"char_{i}",
            stance=stance,
            attributes=attributes,
            context=context,
            decision=decision,
            outcome_score=outcome_score,
            turn=random.randint(1, 100)
        ))

    return samples


if __name__ == '__main__':
    print("PowerGame AI Trainer - Generating training data...")
    training_data = generate_synthetic_training_data(2000)

    trainer = AITrainer()
    for sample in training_data:
        trainer.add_training_sample(sample)

    trainer.split_data(validation_ratio=0.2)

    print(f"Training samples: {len(trainer.training_data)}")
    print(f"Validation samples: {len(trainer.validation_data)}")

    print("\nTraining model...")
    history = trainer.train(epochs=50, learning_rate=0.01)

    print(f"\nFinal training accuracy: {history[-1]['accuracy']:.4f}")
    print(f"Final validation accuracy: {history[-1]['validation_accuracy']:.4f}")
    print(f"Final loss: {history[-1]['loss']:.6f}")

    test_data = generate_synthetic_training_data(500)
    evaluation = trainer.evaluate(test_data)
    print(f"\nTest evaluation:")
    print(f"  Accuracy: {evaluation['accuracy']:.4f}")
    print(f"  Average loss: {evaluation['average_loss']:.6f}")
    print(f"  Per-decision accuracy: {evaluation['per_decision_accuracy']}")

    trainer.export_model('ai_model_weights.json')
    print("\nModel exported to ai_model_weights.json")
