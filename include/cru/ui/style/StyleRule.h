#pragma once
#include "../Base.h"
#include "Condition.h"
#include "Styler.h"
#include "cru/base/ClonablePtr.h"

#include <memory>
#include <vector>

namespace cru::ui::style {
/**
 * \brief An immutable style rule contains a condition and a styler.
 * \remarks This class is immutable and has value semantics.
 */
class CRU_UI_API StyleRule : public Object {
 public:
  static ClonablePtr<StyleRule> Create(ClonablePtr<Condition> condition,
                                       ClonablePtr<Styler> styler,
                                       String name = {}) {
    return ClonablePtr<StyleRule>(new StyleRule(
        std::move(condition), std::move(styler), std::move(name)));
  }

  StyleRule(ClonablePtr<Condition> condition, ClonablePtr<Styler> styler,
            String name = {});

  CRU_DEFAULT_COPY(StyleRule)
  CRU_DEFAULT_MOVE(StyleRule)

  ~StyleRule() override = default;

 public:
  String GetName() const { return name_; }
  Condition* GetCondition() const { return condition_.get(); }
  Styler* GetStyler() const { return styler_.get(); }

  StyleRule WithNewCondition(ClonablePtr<Condition> condition,
                             String name = {}) const {
    return StyleRule{std::move(condition), styler_, std::move(name)};
  }

  StyleRule WithNewStyler(ClonablePtr<Styler> styler, String name = {}) const {
    return StyleRule{condition_, std::move(styler), std::move(name)};
  }

  bool CheckAndApply(controls::Control* control) const;

 private:
  ClonablePtr<Condition> condition_;
  ClonablePtr<Styler> styler_;
  String name_;
};
}  // namespace cru::ui::style
