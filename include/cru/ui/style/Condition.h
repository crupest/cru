#pragma once
#include "../Base.h"
#include "cru/common/Base.h"
#include "cru/common/ClonablePtr.h"
#include "cru/common/Event.h"
#include "cru/ui/controls/IClickableControl.h"
#include "cru/ui/helper/ClickDetector.h"

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace cru::ui::style {
class CRU_UI_API Condition : public Object {
 public:
  virtual std::vector<IBaseEvent*> ChangeOn(
      controls::Control* control) const = 0;
  virtual bool Judge(controls::Control* control) const = 0;

  virtual Condition* Clone() const = 0;
};

class CRU_UI_API NoCondition : public Condition {
 public:
  static ClonablePtr<NoCondition> Create() {
    return ClonablePtr<NoCondition>(new NoCondition);
  };

  std::vector<IBaseEvent*> ChangeOn(controls::Control*) const override {
    return {};
  }

  bool Judge(controls::Control*) const override { return true; }

  NoCondition* Clone() const override { return new NoCondition; }
};

class CRU_UI_API CompoundCondition : public Condition {
 public:
  explicit CompoundCondition(std::vector<ClonablePtr<Condition>> conditions);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;

 protected:
  std::vector<ClonablePtr<Condition>> conditions_;
};

class CRU_UI_API AndCondition : public CompoundCondition {
 public:
  static ClonablePtr<AndCondition> Create(
      std::vector<ClonablePtr<Condition>> conditions) {
    return ClonablePtr<AndCondition>(new AndCondition(std::move(conditions)));
  }

  using CompoundCondition::CompoundCondition;

  bool Judge(controls::Control* control) const override;

  AndCondition* Clone() const override { return new AndCondition(conditions_); }
};

class CRU_UI_API OrCondition : public CompoundCondition {
 public:
  static ClonablePtr<OrCondition> Create(
      std::vector<ClonablePtr<Condition>> conditions) {
    return ClonablePtr<OrCondition>(new OrCondition(std::move(conditions)));
  }

  using CompoundCondition::CompoundCondition;

  bool Judge(controls::Control* control) const override;

  OrCondition* Clone() const override { return new OrCondition(conditions_); }
};

class CRU_UI_API FocusCondition : public Condition {
 public:
  static ClonablePtr<FocusCondition> Create(bool has_focus) {
    return ClonablePtr<FocusCondition>(new FocusCondition(has_focus));
  }

  explicit FocusCondition(bool has_focus);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;
  bool Judge(controls::Control* control) const override;

  FocusCondition* Clone() const override {
    return new FocusCondition(has_focus_);
  }

  bool IsHasFocus() const { return has_focus_; }

 private:
  bool has_focus_;
};

class CRU_UI_API HoverCondition : public Condition {
 public:
  static ClonablePtr<HoverCondition> Create(bool hover) {
    return ClonablePtr<HoverCondition>(new HoverCondition(hover));
  }

  explicit HoverCondition(bool hover) : hover_(hover) {}

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;
  bool Judge(controls::Control* control) const override;

  HoverCondition* Clone() const override { return new HoverCondition(hover_); }

 private:
  bool hover_;
};

class CRU_UI_API ClickStateCondition : public Condition {
 public:
  static ClonablePtr<ClickStateCondition> Create(
      helper::ClickState click_state) {
    return ClonablePtr<ClickStateCondition>(
        new ClickStateCondition(click_state));
  }

  explicit ClickStateCondition(helper::ClickState click_state);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;
  bool Judge(controls::Control* control) const override;

  ClickStateCondition* Clone() const override {
    return new ClickStateCondition(click_state_);
  }

  helper::ClickState GetClickState() const { return click_state_; }

 private:
  helper::ClickState click_state_;
};

class CRU_UI_API CheckedCondition : public Condition {
 public:
  static ClonablePtr<CheckedCondition> Create(bool checked) {
    return ClonablePtr<CheckedCondition>(new CheckedCondition(checked));
  }

  explicit CheckedCondition(bool checked);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;
  bool Judge(controls::Control* control) const override;

  CheckedCondition* Clone() const override {
    return new CheckedCondition(checked_);
  }

  bool IsChecked() const { return checked_; }

 private:
  bool checked_;
};
}  // namespace cru::ui::style
