#pragma once

#include <axolotl/types.hh>

namespace axl {

  enum class BehaviourState { Initialized, Failed, Succeeded, Ongoing };

  class BehaviourNode {
   public:
    BehaviourNode(const std::string &name): _name(name), _state(BehaviourState::Initialized) { }
    virtual ~BehaviourNode() { }
    virtual BehaviourState Execute(f32 step) = 0;

    virtual void Reset() {
      _state = BehaviourState::Initialized;
    }

   protected:
    BehaviourState _state;
    std::string _name;
  };

  class BehaviourNodeWithChildren: public BehaviourNode {
   public:
    BehaviourNodeWithChildren(const std::string &name): BehaviourNode(name) { }

    ~BehaviourNodeWithChildren() { }

    BehaviourNode *AddChild(BehaviourNode *child) {
      _children.push_back(std::unique_ptr<BehaviourNode>(child));
      return _children.back().get();
    }

    void Reset() override {
      for (auto &child : _children)
        child->Reset();
    }

   protected:
    std::vector<std::unique_ptr<BehaviourNode>> _children;
  };

  class BehaviourSelector: public BehaviourNodeWithChildren {
   public:
    BehaviourSelector(const std::string &name): BehaviourNodeWithChildren(name) { }

    ~BehaviourSelector() { }

    virtual BehaviourState Execute(f32 step) override {
      for (auto &child : _children) {
        auto state = child->Execute(step);
        switch (state) {
          case BehaviourState::Failed:
            continue;
          case BehaviourState::Initialized:
          case BehaviourState::Succeeded:
          case BehaviourState::Ongoing:
            _state = state;
            return _state;
        }
      }
      return BehaviourState::Failed;
    }
  };

  class BehaviourSequence: public BehaviourNodeWithChildren {
   public:
    BehaviourSequence(const std::string &name): BehaviourNodeWithChildren(name) { }

    ~BehaviourSequence() { }

    BehaviourState Execute(f32 step) override {
      for (auto &child : _children) {
        auto state = child->Execute(step);
        switch (state) {
          case BehaviourState::Succeeded:
            continue;
          case BehaviourState::Initialized:
          case BehaviourState::Failed:
          case BehaviourState::Ongoing:
            _state = state;
            return _state;
        }
      }
      return BehaviourState::Succeeded;
    }
  };

  typedef std::function<BehaviourState(f32, BehaviourState)> BehaviourFunc;

  class BehaviourAction: public BehaviourNode {
   public:
    BehaviourAction(const std::string &name, BehaviourFunc func): BehaviourNode(name), _func(func) { }

    ~BehaviourAction() { }

    BehaviourState Execute(f32 step) override {
      _state = _func(step, _state);
      return _state;
    }

   protected:
    BehaviourFunc _func;
  };

  class BehaviourInverter: public BehaviourNode {
   public:
    BehaviourInverter(const std::string &name): BehaviourNode(name) { }

    ~BehaviourInverter() { }

    BehaviourState Execute(f32 step) override {
      auto state = _child->Execute(step);
      switch (state) {
        case BehaviourState::Failed:
          _state = BehaviourState::Succeeded;
          return _state;
        case BehaviourState::Succeeded:
          _state = BehaviourState::Failed;
          return _state;
        case BehaviourState::Initialized:
        case BehaviourState::Ongoing:
          _state = state;
          return _state;
      }
      return BehaviourState::Failed;
    }

    BehaviourNode *SetChild(BehaviourNode *child) {
      _child = std::unique_ptr<BehaviourNode>(child);
      return _child.get();
    }

   protected:
    std::unique_ptr<BehaviourNode> _child;
  };

} // namespace axl
