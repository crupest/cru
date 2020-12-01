#pragma once
#include "../Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/ui/controls/IClickableControl.hpp"
#include "cru/ui/helper/ClickDetector.hpp"

#include <utility>
#include <vector>

namespace cru::ui::style {
class Trigger {
 public:
  virtual ~Trigger() = default;

  bool GetState() const { return current_; }
  IEvent<bool>* ChangeEvent() { return &change_event_; }

 protected:
  void Raise(bool value);

 private:
  bool current_ = false;
  Event<bool> change_event_;

 protected:
  EventRevokerListGuard guard_;
};

class CompoundTrigger : public Trigger {
 public:
  explicit CompoundTrigger(std::vector<Trigger*> triggers);

  const std::vector<Trigger*>& GetTriggers() const { return triggers_; }

 protected:
  virtual bool CalculateState(const std::vector<Trigger*>& triggers) const = 0;

 private:
  std::vector<Trigger*> triggers_;
};

class AndTrigger : public CompoundTrigger {
 public:
  using CompoundTrigger::CompoundTrigger;

 protected:
  bool CalculateState(const std::vector<Trigger*>& triggers) const override;
};

class OrTrigger : public CompoundTrigger {
 public:
  using CompoundTrigger::CompoundTrigger;

 protected:
  bool CalculateState(const std::vector<Trigger*>& triggers) const override;
};

class FocusTrigger : public Trigger {
 public:
  FocusTrigger(controls::Control* control, bool has_focus);

 private:
  bool has_focus_;
};

class ClickStateTrigger : public Trigger {
 public:
  ClickStateTrigger(controls::IClickableControl* control,
                    helper::ClickState click_state);

 private:
  helper::ClickState click_state_;
};
}  // namespace cru::ui::style
