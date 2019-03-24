#include "render_object.hpp"

#include "cru_debug.hpp"

namespace cru::ui::render {
void RenderObject::AddChild(RenderObject* render_object, const int position) {
  assert(render_object->GetParent() ==
         nullptr);                       // Render object already has a parent.
  assert(position >= 0);                 // Position index is less than 0.
  assert(position <= children_.size());  // Position index is out of bound.

  children_.insert(children_.cbegin() + position, render_object);
  render_object->SetParent(this);
  OnAddChild(render_object, position);
}

void RenderObject::RemoveChild(const int position) {
  assert(position >= 0);                // Position index is less than 0.
  assert(position < children_.size());  // Position index is out of bound.

  const auto i = children_.cbegin() + position;
  const auto removed_child = *i;
  children_.erase(i);
  removed_child->SetParent(nullptr);
  OnRemoveChild(removed_child, position);
}

void RenderObject::Measure(const Size& available_size) {
  OnMeasureCore(available_size);
}

void RenderObject::Layout(const Rect& rect) {
  SetOffset(rect.GetLeftTop());
  SetSize(rect.GetSize());
  OnLayoutCore(Rect{Point::Zero(), rect.GetSize()});
}

void RenderObject::OnParentChanged(RenderObject* old_parent,
                                   RenderObject* new_parent) {}

void RenderObject::OnAddChild(RenderObject* new_child, int position) {}

void RenderObject::OnRemoveChild(RenderObject* removed_child, int position) {}

void RenderObject::OnSizeChanged(const Size& old_size, const Size& new_size) {}

void RenderObject::OnMeasureCore(const Size& available_size) {
  Size margin_padding_size{
      margin_.GetHorizontalTotal() + padding_.GetHorizontalTotal(),
      margin_.GetVerticalTotal() + padding_.GetVerticalTotal()};

  auto coerced_margin_padding_size = margin_padding_size;
  if (coerced_margin_padding_size.width > available_size.width) {
    debug::DebugMessage(
        L"Measure: horizontal length of padding and margin is bigger than "
        L"available length.");
    coerced_margin_padding_size.width = available_size.width;
  }
  if (coerced_margin_padding_size.height > available_size.height) {
    debug::DebugMessage(
        L"Measure: vertical length of padding and margin is bigger than "
        L"available length.");
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
    debug::DebugMessage(
        L"Layout: horizontal length of padding and margin is bigger than "
        L"available length.");
    coerced_content_available_size.width = 0;
  }
  if (coerced_content_available_size.height < 0) {
    debug::DebugMessage(
        L"Layout: vertical length of padding and margin is bigger than "
        L"available length.");
    coerced_content_available_size.height = 0;
  }

  OnLayoutContent(Rect{margin_.left + padding_.left, margin_.top + padding_.top,
                       coerced_content_available_size.width,
                       coerced_content_available_size.height});
}

void RenderObject::SetParent(RenderObject* new_parent) {
  const auto old_parent = parent_;
  parent_ = new_parent;
  OnParentChanged(old_parent, new_parent);
}
}  // namespace cru::ui::render
