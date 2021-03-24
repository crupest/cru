#include "cru/ui/style/StyleRule.hpp"

namespace cru::ui::style {
StyleRule::StyleRule(ClonablePtr<Condition> condition,
                     ClonablePtr<Styler> styler, std::u16string name)
    : condition_(std::move(condition)),
      styler_(std::move(styler)),
      name_(std::move(name)) {}

bool StyleRule::CheckAndApply(controls::Control *control) const {
  auto active = condition_->Judge(control);
  if (active) {
    styler_->Apply(control);
  }
  return active;
}
}  // namespace cru::ui::style
