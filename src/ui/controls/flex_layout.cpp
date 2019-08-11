#include "cru/ui/controls/flex_layout.hpp"

#include "cru/ui/render/flex_layout_render_object.hpp"

namespace cru::ui::controls {
using render::FlexLayoutRenderObject;

FlexLayout::FlexLayout() {
  render_object_.reset(new FlexLayoutRenderObject());
  render_object_->SetAttachedControl(this);
}

render::RenderObject* FlexLayout::GetRenderObject() const {
  return render_object_.get();
}

void FlexLayout::OnAddChild(Control* child, int position) {
  render_object_->AddChild(child->GetRenderObject(), position);
}

void FlexLayout::OnRemoveChild(Control* child, int position) {
  render_object_->RemoveChild(position);
}
}  // namespace cru::ui::controls
