#include "cru/ui/controls/FlexLayout.h"

#include "cru/ui/render/FlexLayoutRenderObject.h"

namespace cru::ui::controls {
using render::FlexLayoutRenderObject;

FlexLayout::FlexLayout() {
  render_object_.reset(new FlexLayoutRenderObject());
  render_object_->SetAttachedControl(this);
  SetContainerRenderObject(render_object_.get());
}

FlexLayout::~FlexLayout() = default;

render::RenderObject* FlexLayout::GetRenderObject() const {
  return render_object_.get();
}

namespace {
Index FindPosition(render::RenderObject* parent, render::RenderObject* child) {
  const auto& render_objects = parent->GetChildren();
  const auto find_result =
      std::find(render_objects.cbegin(), render_objects.cend(), child);
  if (find_result == render_objects.cend()) {
    throw std::logic_error("Control is not a child of FlexLayout.");
  }
  return static_cast<Index>(find_result - render_objects.cbegin());
}
}  // namespace

FlexChildLayoutData FlexLayout::GetChildLayoutData(Control* control) {
  Expects(control);
  return render_object_->GetChildLayoutData(
      FindPosition(render_object_.get(), control->GetRenderObject()));
}

void FlexLayout::SetChildLayoutData(Control* control,
                                    FlexChildLayoutData data) {
  Expects(control);
  render_object_->SetChildLayoutData(
      FindPosition(render_object_.get(), control->GetRenderObject()),
      std::move(data));
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

FlexCrossAlignment FlexLayout::GetItemCrossAlign() const {
  return render_object_->GetItemCrossAlign();
}

void FlexLayout::SetItemCrossAlign(FlexCrossAlignment alignment) {
  if (alignment == GetItemCrossAlign()) return;
  render_object_->SetItemCrossAlign(alignment);
}
}  // namespace cru::ui::controls
