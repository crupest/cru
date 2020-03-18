#include "cru/ui/controls/flex_layout.hpp"

#include "cru/ui/render/flex_layout_render_object.hpp"

namespace cru::ui::controls {
using render::FlexLayoutRenderObject;

FlexLayout::FlexLayout() {
  render_object_.reset(new FlexLayoutRenderObject());
  render_object_->SetAttachedControl(this);
}

FlexLayout::~FlexLayout() = default;

render::RenderObject* FlexLayout::GetRenderObject() const {
  return render_object_.get();
}

namespace {
int FindPosition(render::RenderObject* parent, render::RenderObject* child) {
  const auto& render_objects = parent->GetChildren();
  const auto find_result =
      std::find(render_objects.cbegin(), render_objects.cend(), child);
  if (find_result == render_objects.cend()) {
    throw std::logic_error("Control is not a child of FlexLayout.");
  }
  return static_cast<int>(find_result - render_objects.cbegin());
}
}  // namespace

FlexChildLayoutData FlexLayout::GetChildLayoutData(Control* control) {
  assert(control);
  return *render_object_->GetChildLayoutData(
      FindPosition(render_object_.get(), control->GetRenderObject()));
}

void FlexLayout::SetChildLayoutData(Control* control,
                                    const FlexChildLayoutData& data) {
  assert(control);
  *render_object_->GetChildLayoutData(
      FindPosition(render_object_.get(), control->GetRenderObject())) = data;
}

FlexMainAlignment FlexLayout::GetContentMainAlign() const {
  return render_object_->GetContentMainAlign();
}

void FlexLayout::SetContentMainAlign(FlexMainAlignment value) {
  if (value == GetContentMainAlign()) return;
  render_object_->SetContentMainAlign(value);
}

FlexDirection FlexLayout::GetFlexDirection() const {
  return render_object_->GetFlexDirection();
}

void FlexLayout::SetFlexDirection(FlexDirection direction) {
  if (direction == GetFlexDirection()) return;
  render_object_->SetFlexDirection(direction);
}

void FlexLayout::OnAddChild(Control* child, int position) {
  render_object_->AddChild(child->GetRenderObject(), position);
}

void FlexLayout::OnRemoveChild(Control* child, int position) {
  CRU_UNUSED(child)

  render_object_->RemoveChild(position);
}
}  // namespace cru::ui::controls
