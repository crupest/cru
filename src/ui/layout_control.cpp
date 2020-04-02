#include "cru/ui/layout_control.hpp"

#include "cru/ui/window.hpp"

namespace cru::ui {
LayoutControl::~LayoutControl() {
  for (const auto child : children_) delete child;
}

void LayoutControl::AddChild(Control* control, const int position) {
  Expects(control->GetParent() == nullptr);  // The control already has a parent.
  Expects(!dynamic_cast<Window*>(control));  // Can't add a window as child.
  Expects(position >= 0 ||
         position <=
             static_cast<int>(
                 this->children_.size()));  // The position is out of range.

  children_.insert(this->children_.cbegin() + position, control);

  control->_SetParent(this);
  control->_SetDescendantWindow(GetWindow());

  OnAddChild(control, position);
}

void LayoutControl::RemoveChild(const int position) {
  Expects(position >= 0 &&
         position <
             static_cast<int>(
                 this->children_.size()));  // The position is out of range.

  const auto i = children_.cbegin() + position;
  const auto child = *i;

  children_.erase(i);

  child->_SetParent(nullptr);
  child->_SetDescendantWindow(nullptr);

  OnRemoveChild(child, position);
}

void LayoutControl::OnAddChild(Control* child, int position) {
  CRU_UNUSED(child)
  CRU_UNUSED(position)
}

void LayoutControl::OnRemoveChild(Control* child, int position) {
  CRU_UNUSED(child)
  CRU_UNUSED(position)
}
}  // namespace cru::ui
