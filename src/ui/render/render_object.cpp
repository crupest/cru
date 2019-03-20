#include "render_object.hpp"

#include "cru_debug.hpp"

namespace cru::ui::render {
void RenderObject::AddChild(RenderObject* render_object, const int position) {
  if (render_object->GetParent() != nullptr)
    throw std::invalid_argument("Render object already has a parent.");

  if (position < 0)
    throw std::invalid_argument("Position index is less than 0.");

  if (static_cast<std::vector<RenderObject*>::size_type>(position) >
      children_.size())
    throw std::invalid_argument("Position index is out of bound.");

  children_.insert(children_.cbegin() + position, render_object);
  render_object->SetParent(this);
  OnAddChild(render_object, position);
}

void RenderObject::RemoveChild(const int position) {
  if (position < 0)
    throw std::invalid_argument("Position index is less than 0.");

  if (static_cast<std::vector<RenderObject*>::size_type>(position) >=
      children_.size())
    throw std::invalid_argument("Position index is out of bound.");

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

void RenderObject::SetParent(RenderObject* new_parent) {
  const auto old_parent = parent_;
  parent_ = new_parent;
  OnParentChanged(old_parent, new_parent);
}

void RenderObject::OnMeasureCore(const Size& available_size) {
  Size margin_padding_size{
      margin_.GetHorizontalTotal() + padding_.GetHorizontalTotal(),
      margin_.GetVerticalTotal() + padding_.GetVerticalTotal()};
  const auto content_available_size = available_size - margin_padding_size;
  auto coerced_content_available_size = content_available_size;

  if (coerced_content_available_size.width < 0) {
    debug::DebugMessage(
        L"Measure: horizontal length of padding and margin is bigger than "
        L"available length.");
    coerced_content_available_size.width = 0;
  }
  if (coerced_content_available_size.height < 0) {
    debug::DebugMessage(
        L"Measure: vertical length of padding and margin is bigger than "
        L"available length.");
    coerced_content_available_size.height = 0;
  }

  const auto actual_content_size =
      OnMeasureContent(coerced_content_available_size);

  SetPreferredSize(margin_padding_size + content_available_size +
                   actual_content_size);
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
}  // namespace cru::ui::render
