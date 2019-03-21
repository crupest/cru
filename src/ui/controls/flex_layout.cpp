#include "flex_layout.hpp"

#include "ui/render/flex_layout_render_object.hpp"

namespace cru::ui::controls {
using render::FlexLayoutRenderObject;

FlexLayout::FlexLayout() { render_object_ = new FlexLayoutRenderObject(); }

FlexLayout::~FlexLayout() { delete render_object_; }

render::RenderObject* FlexLayout::GetRenderObject() const {
  return render_object_;
}

void FlexLayout::OnAddChild(Control* child, int position) {
  render_object_->AddChild(child->GetRenderObject(), position);
}

void FlexLayout::OnRemoveChild(Control* child, int position) {
  render_object_->RemoveChild(position);
}
}  // namespace cru::ui::controls
