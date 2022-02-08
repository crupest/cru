#include "cru/ui/controls/LayoutControl.h"

#include "cru/ui/render/RenderObject.h"

namespace cru::ui::controls {
void LayoutControl::ClearChildren() {
  while (GetChildren().size() > 0) {
    RemoveChild(0);
  }
}

void LayoutControl::OnAddChild(Control* child, Index position) {
  if (container_render_object_ != nullptr) {
    container_render_object_->AddChild(child->GetRenderObject(), position);
  }
}

void LayoutControl::OnRemoveChild(Control* child, Index position) {
  CRU_UNUSED(child)
  if (container_render_object_ != nullptr) {
    container_render_object_->RemoveChild(position);
  }
}
}  // namespace cru::ui::controls
