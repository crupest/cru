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
  CRU_UNUSED(old_child)
  CRU_UNUSED(new_child)
}
}  // namespace cru::ui::controls
