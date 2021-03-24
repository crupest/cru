#include "cru/ui/controls/ContentControl.hpp"

namespace cru::ui::controls {
Control* ContentControl::GetChild() const {
  if (GetChildren().empty()) return nullptr;
  return GetChildren()[0];
}

void ContentControl::SetChild(Control* child) {
  Control* old_child = nullptr;
  if (!GetChildren().empty()) {
    old_child = GetChildren()[0];
    this->RemoveChild(0);
  }
  if (child) {
    this->AddChild(child, 0);
  }
  OnChildChanged(old_child, child);
}

void ContentControl::OnChildChanged(Control* old_child, Control* new_child) {
  if (container_render_object_) {
    if (old_child) {
      container_render_object_->RemoveChild(0);
    }
    if (new_child) {
      container_render_object_->AddChild(new_child->GetRenderObject(), 0);
    }
  }
}
}  // namespace cru::ui::controls
