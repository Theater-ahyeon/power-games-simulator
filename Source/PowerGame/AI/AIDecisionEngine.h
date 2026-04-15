#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <random>

namespace PowerGame {

namespace BehaviorTree {

enum class NodeStatus {
    Success,
    Failure,
    Running
};

class BTNode {
public:
    virtual ~BTNode() = default;
    virtual NodeStatus Execute() = 0;
    virtual void Reset() {}
};

class Sequence : public BTNode {
public:
    NodeStatus Execute() override {
        for (auto& child : children_) {
            auto status = child->Execute();
            if (status != NodeStatus::Success) return status;
        }
        return NodeStatus::Success;
    }
    void AddChild(std::unique_ptr<BTNode> child) { children_.push_back(std::move(child)); }
    void Reset() override { for (auto& c : children_) c->Reset(); }
private:
    std::vector<std::unique_ptr<BTNode>> children_;
};

class Selector : public BTNode {
public:
    NodeStatus Execute() override {
        for (auto& child : children_) {
            auto status = child->Execute();
            if (status != NodeStatus::Failure) return status;
        }
        return NodeStatus::Failure;
    }
    void AddChild(std::unique_ptr<BTNode> child) { children_.push_back(std::move(child)); }
    void Reset() override { for (auto& c : children_) c->Reset(); }
private:
    std::vector<std::unique_ptr<BTNode>> children_;
};

class Decorator : public BTNode {
public:
    explicit Decorator(std::unique_ptr<BTNode> child) : child_(std::move(child)) {}
    void Reset() override { child_->Reset(); }
protected:
    std::unique_ptr<BTNode> child_;
};

class Inverter : public Decorator {
public:
    using Decorator::Decorator;
    NodeStatus Execute() override {
        auto status = child_->Execute();
        if (status == NodeStatus::Success) return NodeStatus::Failure;
        if (status == NodeStatus::Failure) return NodeStatus::Success;
        return NodeStatus::Running;
    }
};

class Repeater : public Decorator {
public:
    Repeater(std::unique_ptr<BTNode> child, int count) : Decorator(std::move(child)), count_(count) {}
    NodeStatus Execute() override {
        for (int i = 0; i < count_; i++) {
            child_->Execute();
        }
        return NodeStatus::Success;
    }
private:
    int count_;
};

using ActionFunc = std::function<NodeStatus()>;

class Action : public BTNode {
public:
    explicit Action(ActionFunc func) : func_(func) {}
    NodeStatus Execute() override { return func_(); }
private:
    ActionFunc func_;
};

using ConditionFunc = std::function<bool()>;

class Condition : public BTNode {
public:
    explicit Condition(ConditionFunc func) : func_(func) {}
    NodeStatus Execute() override { return func_() ? NodeStatus::Success : NodeStatus::Failure; }
private:
    ConditionFunc func_;
};

}

class UtilityAI {
public:
    struct Consideration {
        std::string name;
        std::function<float()> input;
        std::function<float(float)> curve;
        float weight;
    };

    struct Action {
        std::string name;
        std::vector<Consideration> considerations;
        std::function<void()> execute;
        float bonus;
    };

    std::string Evaluate(const std::vector<Action>& actions);
    float CalculateScore(const Action& action);

    static float LinearCurve(float x) { return x; }
    static float QuadraticCurve(float x) { return x * x; }
    static float SqrtCurve(float x) { return std::sqrt(x); }
    static float LogisticCurve(float x) { return 1.0f / (1.0f + std::exp(-10.0f * (x - 0.5f))); }
    static float StepCurve(float x, float threshold) { return x >= threshold ? 1.0f : 0.0f; }

private:
    float CombineConsiderations(const std::vector<Consideration>& considerations);
};

class AIDecisionEngine {
public:
    struct DecisionContext {
        std::string characterId;
        int turn;
        std::unordered_map<std::string, float> resources;
        std::unordered_map<std::string, float> factionPowers;
        std::vector<std::string> recentEvents;
        float politicalTension;
        float personalThreat;
    };

    struct AIAction {
        std::string name;
        float utility;
        std::string reasoning;
    };

    AIAction Decide(const DecisionContext& context);
    std::vector<AIAction> RankActions(const DecisionContext& context);

    void RegisterActionTemplate(const std::string& name,
                                 std::function<float(const DecisionContext&)> utilityFunc,
                                 const std::string& description);

private:
    struct ActionTemplate {
        std::string name;
        std::function<float(const DecisionContext&)> utilityFunc;
        std::string description;
    };

    std::vector<ActionTemplate> actionTemplates_;
};

}
