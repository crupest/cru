#include "content_control.hpp"

#include "window.hpp"

namespace cru::ui {
ContentControl::ContentControl()
    : child_vector_{nullptr}, child_(child_vector_[0]) {}

ContentControl::~ContentControl() { delete child_; }

void ContentControl::SetChild(Control* child) {
  assert(!dynamic_cast<Window*>(child));  // Can't add a window as child.
  if (child == child_) return;

  const auto window = GetWindow();
  const auto old_child = child_;
  child_ = child;
  if (old_child) {
    old_child->_SetParent(nullptr);
    old_child->_SetDescendantWindow(nullptr);
  }
  if (child) {
    child->_SetParent(this);
    child->_SetDescendantWindow(window);
  }
  OnChildChanged(old_child, child);
}

void ContentControl::OnChildChanged(Control* old_child, Control* new_child) {}
}  // namespace cru::ui
