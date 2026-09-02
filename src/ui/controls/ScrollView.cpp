#include "cru/ui/controls/ScrollView.h"

namespace cru::ui::controls {

ScrollView::ScrollView() : Control(kControlName) {}

render::RenderObject* ScrollView::GetRenderObject() {
  return GetScrollRootRenderObject();
}

void ScrollView::OnChildChanged(Control* old_child, Control* new_child) {
  if (old_child) {
    GetScrollRenderObject()->SetChild(nullptr);
  }
  if (new_child) {
    GetScrollRenderObject()->SetChild(new_child->GetRenderObject());
  }
}

}  // namespace cru::ui::controls
