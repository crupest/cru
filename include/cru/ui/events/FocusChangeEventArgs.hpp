#pragma once
#include "UiEventArgs.hpp"

namespace cru::ui::events {

class CRU_UI_API FocusChangeEventArgs : public UiEventArgs {
 public:
  FocusChangeEventArgs(Object* sender, Object* original_sender,
                       const bool is_window = false)
      : UiEventArgs(sender, original_sender), is_window_(is_window) {}
  FocusChangeEventArgs(const FocusChangeEventArgs& other) = default;
  FocusChangeEventArgs(FocusChangeEventArgs&& other) = default;
  FocusChangeEventArgs& operator=(const FocusChangeEventArgs& other) = default;
  FocusChangeEventArgs& operator=(FocusChangeEventArgs&& other) = default;
  ~FocusChangeEventArgs() override = default;

  // Return whether the focus change is caused by the window-wide focus change.
  bool IsWindow() const { return is_window_; }

 private:
  bool is_window_;
};
}  // namespace cru::ui::event
