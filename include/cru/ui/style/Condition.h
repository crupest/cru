#pragma once
#include "../Base.h"
#include <cru/Base.h>
#include <cru/CopyPtr.h>
#include <cru/Event.h>
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
  static CopyPtr<NoCondition> Create() {
    return CopyPtr<NoCondition>(new NoCondition);
  };

  std::vector<IBaseEvent*> ChangeOn(controls::Control*) const override {
    return {};
  }

  bool Judge(controls::Control*) const override { return true; }

  NoCondition* Clone() const override { return new NoCondition; }
};

class CRU_UI_API CompoundCondition : public Condition {
 public:
  explicit CompoundCondition(std::vector<CopyPtr<Condition>> conditions);

  std::vector<IBaseEvent*> ChangeOn(controls::Control* control) const override;

  std::vector<CopyPtr<Condition>> GetChildren() const {
    return conditions_;
  }

 protected:
  std::vector<CopyPtr<Condition>> conditions_;
};

class CRU_UI_API AndCondition : public CompoundCondition {
 public:
  static CopyPtr<AndCondition> Create(
      std::vector<CopyPtr<Condition>> conditions) {
    return CopyPtr<AndCondition>(new AndCondition(std::move(conditions)));
  }

  using CompoundCondition::CompoundCondition;

  bool Judge(controls::Control* control) const override;

  AndCondition* Clone() const override { return new AndCondition(conditions_); }
};

class CRU_UI_API OrCondition : public CompoundCondition {
 public:
  static CopyPtr<OrCondition> Create(
      std::vector<CopyPtr<Condition>> conditions) {
    return CopyPtr<OrCondition>(new OrCondition(std::move(conditions)));
  }

  using CompoundCondition::CompoundCondition;

  bool Judge(controls::Control* control) const override;

  OrCondition* Clone() const override { return new OrCondition(conditions_); }
};

class CRU_UI_API FocusCondition : public Condition {
 public:
  static CopyPtr<FocusCondition> Create(bool has_focus) {
    return CopyPtr<FocusCondition>(new FocusCondition(has_focus));
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
  static CopyPtr<HoverCondition> Create(bool hover) {
    return CopyPtr<HoverCondition>(new HoverCondition(hover));
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
  static CopyPtr<ClickStateCondition> Create(
      helper::ClickState click_state) {
    return CopyPtr<ClickStateCondition>(
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
  static CopyPtr<CheckedCondition> Create(bool checked) {
    return CopyPtr<CheckedCondition>(new CheckedCondition(checked));
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
