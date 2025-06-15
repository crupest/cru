#pragma once
#include <cru/Base.h>
#include <cru/CopyPtr.h>
#include "../Base.h"
#include "Condition.h"
#include "Styler.h"

namespace cru::ui::style {
/**
 * TODO: This design is so strange!
 * \brief An immutable style rule contains a condition and a styler.
 * \remarks This class is immutable and has value semantics.
 */
class CRU_UI_API StyleRule {
 public:
  static CopyPtr<StyleRule> Create(CopyPtr<Condition> condition,
                                   CopyPtr<Styler> styler, String name = {}) {
    return CopyPtr<StyleRule>(new StyleRule(
        std::move(condition), std::move(styler), std::move(name)));
  }

  StyleRule(CopyPtr<Condition> condition, CopyPtr<Styler> styler,
            String name = {});

 public:
  String GetName() const { return name_; }
  Condition* GetCondition() const { return condition_.get(); }
  Styler* GetStyler() const { return styler_.get(); }

  StyleRule WithNewCondition(CopyPtr<Condition> condition,
                             String name = {}) const {
    return StyleRule{std::move(condition), styler_, std::move(name)};
  }

  StyleRule WithNewStyler(CopyPtr<Styler> styler, String name = {}) const {
    return StyleRule{condition_, std::move(styler), std::move(name)};
  }

  bool CheckAndApply(controls::Control* control) const;

 private:
  CopyPtr<Condition> condition_;
  CopyPtr<Styler> styler_;
  String name_;
};
}  // namespace cru::ui::style
