#include "cru/ui/render/RenderObject.hpp"

#include "cru/common/Logger.hpp"
#include "cru/ui/UiHost.hpp"

#include <algorithm>

namespace cru::ui::render {
void RenderObject::AddChild(RenderObject* render_object, const Index position) {
  Expects(child_mode_ != ChildMode::None);
  Expects(!(child_mode_ == ChildMode::Single && children_.size() > 0));

  Expects(render_object->GetParent() ==
          nullptr);        // Render object already has a parent.
  Expects(position >= 0);  // Position index is less than 0.
  Expects(
      position <=
      static_cast<Index>(children_.size()));  // Position index is out of bound.

  children_.insert(children_.cbegin() + position, render_object);
  render_object->SetParent(this);
  render_object->SetRenderHostRecursive(GetUiHost());
  OnAddChild(render_object, position);
}

void RenderObject::RemoveChild(const Index position) {
  Expects(position >= 0);  // Position index is less than 0.
  Expects(position < static_cast<Index>(
                         children_.size()));  // Position index is out of bound.

  const auto i = children_.cbegin() + position;
  const auto removed_child = *i;
  children_.erase(i);
  removed_child->SetParent(nullptr);
  removed_child->SetRenderHostRecursive(nullptr);
  OnRemoveChild(removed_child, position);
}

Point RenderObject::GetTotalOffset() const {
  Point result{};
  const RenderObject* render_object = this;

  while (render_object != nullptr) {
    const auto o = render_object->GetOffset();
    result.x += o.x;
    result.y += o.y;
    render_object = render_object->GetParent();
  }

  return result;
}

Point RenderObject::FromRootToContent(const Point& point) const {
  const auto offset = GetTotalOffset();
  const auto rect = GetContentRect();
  return Point{point.x - (offset.x + rect.left),
               point.y - (offset.y + rect.top)};
}

void RenderObject::Measure(const Size& available_size) {
  OnMeasureCore(available_size);
}

void RenderObject::Layout(const Rect& rect) {
  SetOffset(rect.GetLeftTop());
  SetSize(rect.GetSize());
  OnLayoutCore(Rect{Point{}, rect.GetSize()});
}

void RenderObject::OnParentChanged(RenderObject* old_parent,
                                   RenderObject* new_parent) {
  CRU_UNUSED(old_parent)
  CRU_UNUSED(new_parent)
}

void RenderObject::OnAddChild(RenderObject* new_child, Index position) {
  CRU_UNUSED(new_child)
  CRU_UNUSED(position)

  InvalidateLayout();
  InvalidatePaint();
}

void RenderObject::OnRemoveChild(RenderObject* removed_child, Index position) {
  CRU_UNUSED(removed_child)
  CRU_UNUSED(position)

  InvalidateLayout();
  InvalidatePaint();
}

void RenderObject::OnMeasureCore(const Size& available_size) {
  Size margin_padding_size{
      margin_.GetHorizontalTotal() + padding_.GetHorizontalTotal(),
      margin_.GetVerticalTotal() + padding_.GetVerticalTotal()};

  auto coerced_margin_padding_size = margin_padding_size;
  if (coerced_margin_padding_size.width > available_size.width) {
    log::Warn(
        "Measure: horizontal length of padding and margin is bigger than "
        "available length.");
    coerced_margin_padding_size.width = available_size.width;
  }
  if (coerced_margin_padding_size.height > available_size.height) {
    log::Warn(
        "Measure: vertical length of padding and margin is bigger than "
        "available length.");
    coerced_margin_padding_size.height = available_size.height;
  }

  const auto coerced_content_available_size =
      available_size - coerced_margin_padding_size;
  const auto actual_content_size =
      OnMeasureContent(coerced_content_available_size);

  SetPreferredSize(coerced_margin_padding_size + actual_content_size);
}

void RenderObject::OnLayoutCore(const Rect& rect) {
  Size margin_padding_size{
      margin_.GetHorizontalTotal() + padding_.GetHorizontalTotal(),
      margin_.GetVerticalTotal() + padding_.GetVerticalTotal()};
  const auto content_available_size = rect.GetSize() - margin_padding_size;
  auto coerced_content_available_size = content_available_size;

  if (coerced_content_available_size.width < 0) {
    log::Warn(
        "Layout: horizontal length of padding and margin is bigger than "
        "available length.");
    coerced_content_available_size.width = 0;
  }
  if (coerced_content_available_size.height < 0) {
    log::Warn(
        "Layout: vertical length of padding and margin is bigger than "
        "available length.");
    coerced_content_available_size.height = 0;
  }

  OnLayoutContent(Rect{margin_.left + padding_.left, margin_.top + padding_.top,
                       coerced_content_available_size.width,
                       coerced_content_available_size.height});
}

void RenderObject::OnAfterLayout() {}

Rect RenderObject::GetPaddingRect() const {
  Rect rect{Point{}, GetSize()};
  rect = rect.Shrink(GetMargin());
  rect.width = std::max(rect.width, 0.0f);
  rect.height = std::max(rect.height, 0.0f);
  return rect;
}

Rect RenderObject::GetContentRect() const {
  Rect rect{Point{}, GetSize()};
  rect = rect.Shrink(GetMargin());
  rect = rect.Shrink(GetPadding());
  rect.width = std::max(rect.width, 0.0f);
  rect.height = std::max(rect.height, 0.0f);
  return rect;
}

void RenderObject::SetParent(RenderObject* new_parent) {
  const auto old_parent = parent_;
  parent_ = new_parent;
  OnParentChanged(old_parent, new_parent);
}

void RenderObject::InvalidateLayout() {
  if (ui_host_ != nullptr) ui_host_->InvalidateLayout();
}

void RenderObject::InvalidatePaint() {
  if (ui_host_ != nullptr) ui_host_->InvalidatePaint();
}

void RenderObject::NotifyAfterLayoutRecursive(RenderObject* render_object) {
  render_object->OnAfterLayout();
  for (const auto o : render_object->GetChildren()) {
    NotifyAfterLayoutRecursive(o);
  }
}

void RenderObject::SetRenderHostRecursive(UiHost* host) {
  ui_host_ = host;
  for (const auto child : GetChildren()) {
    child->SetRenderHostRecursive(host);
  }
}
}  // namespace cru::ui::render