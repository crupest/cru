#pragma once
#include "../Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/common/Event.hpp"
#include "cru/ui/controls/IClickableControl.hpp"
#include "cru/ui/helper/ClickDetector.hpp"

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace cru::ui::style {
class Condition : public Object {
 public:
  virtual std::vector<IBaseEvent*> ChangeOn(
      controls::Control* control) const = 0;
  virtual bool Judge(controls::Control* control) const = 0;

  virtual Condition* Clone() const = 0;
};

class CompoundCondition : public Condition {
 public:
  explicit CompoundCondition(std::vector<ClonablePtr<Condition>> conditions);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;

 protected:
  std::vector<ClonablePtr<Condition>> conditions_;
};

class AndCondition : public CompoundCondition {
 public:
  using CompoundCondition::CompoundCondition;

  bool Judge(controls::Control* control) const override;

  AndCondition* Clone() const override { return new AndCondition(conditions_); }
};

class OrCondition : public CompoundCondition {
 public:
  using CompoundCondition::CompoundCondition;

  bool Judge(controls::Control* control) const override;

  OrCondition* Clone() const override { return new OrCondition(conditions_); }
};

class FocusCondition : public Condition {
 public:
  explicit FocusCondition(bool has_focus);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;
  bool Judge(controls::Control* control) const override;

  FocusCondition* Clone() const override {
    return new FocusCondition(has_focus_);
  }

 private:
  bool has_focus_;
};

class ClickStateCondition : public Condition {
 public:
  explicit ClickStateCondition(helper::ClickState click_state);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;
  bool Judge(controls::Control* control) const override;

  ClickStateCondition* Clone() const override {
    return new ClickStateCondition(click_state_);
  }

 private:
  helper::ClickState click_state_;
};
}  // namespace cru::ui::style
