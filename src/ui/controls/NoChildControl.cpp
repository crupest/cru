#include "cru/ui/controls/NoChildControl.h"

namespace cru::ui::controls {
void NoChildControl::ForEachChild(
    const std::function<void(Control*)>& callback) {
  CRU_UNUSED(callback);
}

void NoChildControl::RemoveChild(Control* child) { CRU_UNUSED(child); }
}  // namespace cru::ui::controls
