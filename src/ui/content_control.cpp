#include "cru/ui/content_control.hpp"

#include "cru/ui/window.hpp"

namespace cru::ui {
ContentControl::ContentControl()
    : child_vector_{nullptr}, child_(child_vector_[0]) {}

ContentControl::~ContentControl() { delete child_; }

void ContentControl::SetChild(Control* child) {
  Expects(!dynamic_cast<Window*>(child));  // Can't add a window as child.
  if (child == child_) return;

  const auto host = GetUiHost();
  const auto old_child = child_;
  child_ = child;
  if (old_child) {
    old_child->_SetParent(nullptr);
    old_child->_SetDescendantUiHost(nullptr);
  }
  if (child) {
    child->_SetParent(this);
    child->_SetDescendantUiHost(host);
  }
  OnChildChanged(old_child, child);
}

void ContentControl::OnChildChanged(Control* old_child, Control* new_child) {
  CRU_UNUSED(old_child)
  CRU_UNUSED(new_child)
}
}  // namespace cru::ui
