#include "layout_control.hpp"

#include "window.hpp"

namespace cru::ui {
void ControlAddChildCheck(Control* control) {
  if (control->GetParent() != nullptr)
    throw std::invalid_argument("The control already has a parent.");

  if (dynamic_cast<Window*>(control))
    throw std::invalid_argument("Can't add a window as child.");
}

LayoutControl::~LayoutControl() {
  for (const auto child : children_) delete child;
}

void LayoutControl::AddChild(Control* control, const int position) {
  ControlAddChildCheck(control);

  if (position < 0 || static_cast<decltype(children_.size())>(position) >
                          this->children_.size())
    throw std::invalid_argument("The position is out of range.");

  children_.insert(this->children_.cbegin() + position, control);

  control->_SetParent(this);
  control->_SetDescendantWindow(GetWindow());

  OnAddChild(control, position);
}

void LayoutControl::RemoveChild(const int position) {
  if (position < 0 || static_cast<decltype(this->children_.size())>(position) >=
                          this->children_.size())
    throw std::invalid_argument("The position is out of range.");

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
