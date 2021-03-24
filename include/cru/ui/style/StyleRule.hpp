#pragma once
#include "Condition.hpp"
#include "Styler.hpp"
#include "cru/common/Base.hpp"
#include "cru/common/ClonablePtr.hpp"
#include "cru/ui/Base.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cru::ui::style {
class StyleRule : public Object {
 public:
  StyleRule(ClonablePtr<Condition> condition, ClonablePtr<Styler> styler,
            std::u16string name = {});

  CRU_DEFAULT_COPY(StyleRule)
  CRU_DEFAULT_MOVE(StyleRule)

  ~StyleRule() override = default;

 public:
  const std::u16string& GetName() const { return name_; }
  Condition* GetCondition() const { return condition_.get(); }
  Styler* GetStyler() const { return styler_.get(); }

  StyleRule WithNewCondition(ClonablePtr<Condition> condition,
                             std::u16string name = {}) const {
    return StyleRule{std::move(condition), styler_, std::move(name)};
  }

  StyleRule WithNewStyler(ClonablePtr<Styler> styler,
                          std::u16string name = {}) const {
    return StyleRule{condition_, std::move(styler), std::move(name)};
  }

  bool CheckAndApply(controls::Control* control) const;

 private:
  ClonablePtr<Condition> condition_;
  ClonablePtr<Styler> styler_;
  std::u16string name_;
};
}  // namespace cru::ui::style
