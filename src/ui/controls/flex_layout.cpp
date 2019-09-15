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

FlexChildLayoutData FlexLayout::GetChildLayoutData(Control* control) {
  const auto& render_objects = render_object_->GetChildren();
  const auto find_result =
      std::find(render_objects.cbegin(), render_objects.cend(),
                control->GetRenderObject());
  if (find_result == render_objects.cend()) {
    throw std::logic_error("Control is not a child of FlexLayout.");
  }
  int position = static_cast<int>(find_result - render_objects.cbegin());
  return *(render_object_->GetChildLayoutData(position));
}

void FlexLayout::SetChildLayoutData(Control* control,
                                    const FlexChildLayoutData& data) {
  const auto& render_objects = render_object_->GetChildren();
  const auto find_result =
      std::find(render_objects.cbegin(), render_objects.cend(),
                control->GetRenderObject());
  if (find_result == render_objects.cend()) {
    throw std::logic_error("Control is not a child of FlexLayout.");
  }
  int position = static_cast<int>(find_result - render_objects.cbegin());
  const auto d = render_object_->GetChildLayoutData(position);
  *d = data;
  if (const auto window = GetWindow()) window->InvalidateLayout();
}

void FlexLayout::OnAddChild(Control* child, int position) {
  render_object_->AddChild(child->GetRenderObject(), position);
}

void FlexLayout::OnRemoveChild(Control* child, int position) {
  render_object_->RemoveChild(position);
}
}  // namespace cru::ui::controls
