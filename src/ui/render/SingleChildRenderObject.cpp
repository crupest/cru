#include "cru/ui/render/SingleChildRenderObject.h"

namespace cru::ui::render {
void SingleChildRenderObject::SetChild(RenderObject *new_child) {
  assert(new_child == nullptr || new_child->GetParent() == nullptr);
  if (child_ == new_child) return;
  auto old_child = child_;
  if (child_) {
    child_->SetParent(nullptr);
  }
  child_ = new_child;
  if (child_) {
    child_->SetParent(this);
  }
  OnChildChanged(old_child, new_child);
}

void SingleChildRenderObject::OnChildChanged(RenderObject *old_child,
                                             RenderObject *new_child) {
  InvalidateLayout();
}
}  // namespace cru::ui::render
