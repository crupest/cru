#pragma once
#include "../Base.hpp"
#include "Condition.hpp"
#include "Styler.hpp"
#include "cru/common/ClonablePtr.hpp"

#include <memory>
#include <vector>

namespace cru::ui::style {
class StyleRule : public Object {
 public:
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
