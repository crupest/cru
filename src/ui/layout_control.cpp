#include "layout_control.hpp"

#include "window.hpp"

namespace cru::ui {
LayoutControl::~LayoutControl() {
  for (const auto child : children_) delete child;
}

void LayoutControl::AddChild(Control* control, const int position) {
  assert(control->GetParent() == nullptr);  // The control already has a parent.
  assert(!dynamic_cast<Window*>(control));  // Can't add a window as child.
  assert(position >= 0 || position <= this->children_.size()); // The position is out of range.

  children_.insert(this->children_.cbegin() + position, control);

  control->_SetParent(this);
  control->_SetDescendantWindow(GetWindow());

  OnAddChild(control, position);
}

void LayoutControl::RemoveChild(const int position) {
  assert(position >= 0 &&
         position < this->children_.size());  // The position is out of range.

  const auto i = children_.cbegin() + position;
  const auto child = *i;

  children_.erase(i);

  child->_SetParent(nullptr);
  child->_SetDescendantWindow(nullptr);

  OnRemoveChild(child, position);
}

void LayoutControl::OnAddChild(Control* child, int position) {}

void LayoutControl::OnRemoveChild(Control* child, int position) {}
}  // namespace cru::ui
