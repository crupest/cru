#pragma once
#include "../Base.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/Event.hpp"
#include "cru/ui/controls/IClickableControl.hpp"
#include "cru/ui/helper/ClickDetector.hpp"

#include <type_traits>
#include <utility>
#include <vector>

namespace cru::ui::style {
class Condition : public Object {
 public:
  virtual std::vector<IBaseEvent*> ChangeOn(
      controls::Control* control) const = 0;
  virtual bool Judge(controls::Control* control) const = 0;
};

class CompoundCondition : public Condition {
 public:
  explicit CompoundCondition(std::vector<Condition*> conditions);

  const std::vector<Condition*>& GetConditions() const { return conditions_; }

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;

 private:
  std::vector<Condition*> conditions_;
};

class AndCondition : public CompoundCondition {
 public:
  using CompoundCondition::CompoundCondition;

  bool Judge(controls::Control* control) const override;
};

class OrCondition : public CompoundCondition {
 public:
  using CompoundCondition::CompoundCondition;

  bool Judge(controls::Control* control) const override;
};

class FocusCondition : public Condition {
 public:
  explicit FocusCondition(bool has_focus);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;
  bool Judge(controls::Control* control) const override;

 private:
  bool has_focus_;
};

class ClickStateCondition : public Condition {
 public:
  explicit ClickStateCondition(helper::ClickState click_state);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;
  bool Judge(controls::Control* control) const override;

 private:
  helper::ClickState click_state_;
};
}  // namespace cru::ui::style
