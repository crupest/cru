#pragma once
#include "../Base.h"
#include "Condition.h"
#include "Styler.h"
#include "cru/base/ClonePtr.h"

namespace cru::ui::style {
/**
 * \brief An immutable style rule contains a condition and a styler.
 * \remarks This class is immutable and has value semantics.
 */
class CRU_UI_API StyleRule {
 public:
  static ClonePtr<StyleRule> Create(ClonePtr<Condition> condition,
                                       ClonePtr<Styler> styler,
                                       std::string name = {}) {
    return ClonePtr<StyleRule>(new StyleRule(
        std::move(condition), std::move(styler), std::move(name)));
  }

  StyleRule(ClonePtr<Condition> condition, ClonePtr<Styler> styler,
            std::string name = {});

 public:
  std::string GetName() const { return name_; }
  Condition* GetCondition() const { return condition_.get(); }
  Styler* GetStyler() const { return styler_.get(); }

  StyleRule WithNewCondition(ClonePtr<Condition> condition,
                             std::string name = {}) const {
    return StyleRule{std::move(condition), styler_, std::move(name)};
  }

  StyleRule WithNewStyler(ClonePtr<Styler> styler,
                          std::string name = {}) const {
    return StyleRule{condition_, std::move(styler), std::move(name)};
  }

  bool CheckAndApply(controls::Control* control) const;

 private:
  ClonePtr<Condition> condition_;
  ClonePtr<Styler> styler_;
  std::string name_;
};
}  // namespace cru::ui::style
