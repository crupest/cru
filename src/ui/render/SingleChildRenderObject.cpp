#include "cru/ui/render/SingleChildRenderObject.h"
#include "cru/ui/render/RenderObject.h"

namespace cru::ui::render {
SingleChildRenderObject::SingleChildRenderObject(std::string name)
    : RenderObject(std::move(name)) {}

SingleChildRenderObject::~SingleChildRenderObject() { SetChild(nullptr); }

void SingleChildRenderObject::SetChild(RenderObject* new_child) {
  assert(new_child == nullptr || new_child->GetParent() == nullptr);
  if (child_ == new_child) return;
  auto old_child = child_;
  if (child_) {
    child_->SetParent(nullptr);
    child_destroy_guard_.Reset();
  }
  child_ = new_child;
  if (child_) {
    child_->SetParent(this);
    child_destroy_guard_.Reset(child_->DestroyEvent()->AddSpyOnlyHandler(
        [this] { SetChild(nullptr); }));
  }
  OnChildChanged(old_child, new_child);
}

void SingleChildRenderObject::OnChildChanged(RenderObject* old_child,
                                             RenderObject* new_child) {
  InvalidateLayout();
}
}  // namespace cru::ui::render
