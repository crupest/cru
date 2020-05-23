#include "cru/ui/controls/StackLayout.hpp"

#include "cru/ui/render/StackLayoutRenderObject.hpp"

namespace cru::ui::controls {
using render::StackLayoutRenderObject;

StackLayout::StackLayout() : render_object_(new StackLayoutRenderObject()) {
  render_object_->SetAttachedControl(this);
}

StackLayout::~StackLayout() = default;

render::RenderObject* StackLayout::GetRenderObject() const {
  return render_object_.get();
}

void StackLayout::OnAddChild(Control* child, const Index position) {
  render_object_->AddChild(child->GetRenderObject(), position);
}

void StackLayout::OnRemoveChild(Control* child, const Index position) {
  CRU_UNUSED(child)

  render_object_->RemoveChild(position);
}
}  // namespace cru::ui::controls
