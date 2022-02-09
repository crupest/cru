#include "cru/ui/controls/NoChildControl.h"

namespace cru::ui::controls {
void NoChildControl::ForEachChild(
    const std::function<void(Control*)>& callback) {
  CRU_UNUSED(callback);
}
}  // namespace cru::ui::controls
