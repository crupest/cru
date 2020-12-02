#pragma once
#include "Condition.hpp"
#include "Styler.hpp"
#include "cru/common/Base.hpp"
#include "cru/ui/Base.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cru::ui::style {
class StyleRule : public Object {
 public:
  StyleRule(std::unique_ptr<Condition> condition,
            std::unique_ptr<Styler> styler, std::u16string name = {});

  StyleRule(const StyleRule& other)
      : condition_(other.condition_->Clone()),
        styler_(other.styler_->Clone()),
        name_(other.name_) {}

  StyleRule& operator=(const StyleRule& other) {
    if (this != &other) {
      condition_ = other.condition_->Clone();
      styler_ = other.styler_->Clone();
      name_ = other.name_;
    }
    return *this;
  }

  CRU_DEFAULT_MOVE(StyleRule)

  ~StyleRule() override = default;

 public:
  const std::u16string& GetName() const { return name_; }
  Condition* GetCondition() const { return condition_.get(); }
  Styler* GetStyler() const { return styler_.get(); }

  StyleRule WithNewCondition(std::unique_ptr<Condition> condition,
                             std::u16string name = {}) const {
    return StyleRule{std::move(condition), styler_->Clone(), std::move(name)};
  }

  StyleRule WithNewStyler(std::unique_ptr<Styler> styler,
                          std::u16string name = {}) const {
    return StyleRule{condition_->Clone(), std::move(styler), std::move(name)};
  }

  bool CheckAndApply(controls::Control* control) const;

 private:
  std::unique_ptr<Condition> condition_;
  std::unique_ptr<Styler> styler_;
  std::u16string name_;
};
}  // namespace cru::ui::style
