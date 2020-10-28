#include "cru/ui/ContentControl.hpp"

#include "cru/ui/Window.hpp"

namespace cru::ui {
ContentControl::ContentControl()
    : child_vector_{nullptr}, child_(child_vector_[0]) {}

ContentControl::~ContentControl() { delete child_; }

void ContentControl::SetChild(Control* child) {
  Expects(!dynamic_cast<Window*>(child));  // Can't add a window as child.
  if (child == child_) return;

  const auto host = GetWindowHost();
  const auto old_child = child_;
  child_ = child;
  if (old_child) {
    old_child->_SetParent(nullptr);
    old_child->_SetDescendantWindowHost(nullptr);
  }
  if (child) {
    child->_SetParent(this);
    child->_SetDescendantWindowHost(host);
  }
  OnChildChanged(old_child, child);
}

void ContentControl::OnChildChanged(Control* old_child, Control* new_child) {
  CRU_UNUSED(old_child)
  CRU_UNUSED(new_child)
}
}  // namespace cru::ui
