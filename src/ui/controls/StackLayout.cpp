#include "cru/ui/controls/StackLayout.h"

namespace cru::ui::controls {
StackLayout::StackLayout()
    : LayoutControl<render::StackLayoutRenderObject>(kControlName) {}

StackLayout::~StackLayout() {
  RemoveFromParent();
  RemoveAllChild();
}
}  // namespace cru::ui::controls
