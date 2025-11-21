#include "cru/ui/controls/FlexLayout.h"

namespace cru::ui::controls {
FlexLayout::FlexLayout()
    : LayoutControl<render::FlexLayoutRenderObject>(kControlName) {}

FlexMainAlignment FlexLayout::GetContentMainAlign() {
  return GetContainerRenderObject()->GetContentMainAlign();
}

void FlexLayout::SetContentMainAlign(FlexMainAlignment value) {
  GetContainerRenderObject()->SetContentMainAlign(value);
}

FlexDirection FlexLayout::GetFlexDirection() {
  return GetContainerRenderObject()->GetFlexDirection();
}

void FlexLayout::SetFlexDirection(FlexDirection direction) {
  GetContainerRenderObject()->SetFlexDirection(direction);
}

FlexCrossAlignment FlexLayout::GetItemCrossAlign() {
  return GetContainerRenderObject()->GetItemCrossAlign();
}

void FlexLayout::SetItemCrossAlign(FlexCrossAlignment alignment) {
  GetContainerRenderObject()->SetItemCrossAlign(alignment);
}
}  // namespace cru::ui::controls
