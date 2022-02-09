#include "cru/ui/controls/FlexLayout.h"

namespace cru::ui::controls {
FlexLayout::FlexLayout() = default;

FlexLayout::~FlexLayout() = default;

FlexMainAlignment FlexLayout::GetContentMainAlign() const {
  return GetContainerRenderObject()->GetContentMainAlign();
}

void FlexLayout::SetContentMainAlign(FlexMainAlignment value) {
  GetContainerRenderObject()->SetContentMainAlign(value);
}

FlexDirection FlexLayout::GetFlexDirection() const {
  return GetContainerRenderObject()->GetFlexDirection();
}

void FlexLayout::SetFlexDirection(FlexDirection direction) {
  GetContainerRenderObject()->SetFlexDirection(direction);
}

FlexCrossAlignment FlexLayout::GetItemCrossAlign() const {
  return GetContainerRenderObject()->GetItemCrossAlign();
}

void FlexLayout::SetItemCrossAlign(FlexCrossAlignment alignment) {
  GetContainerRenderObject()->SetItemCrossAlign(alignment);
}
}  // namespace cru::ui::controls
