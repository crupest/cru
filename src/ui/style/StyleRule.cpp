#include "cru/ui/style/StyleRule.hpp"

namespace cru::ui::style {
bool StyleRule::CheckAndApply(controls::Control *control) const {
  auto active = condition_->Judge(control);
  if (active) {
    styler_->Apply(control);
  }
  return active;
}
}  // namespace cru::ui::style
