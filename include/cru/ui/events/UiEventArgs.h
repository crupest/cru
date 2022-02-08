#pragma once
#include "../Base.h"

namespace cru::ui::events {
class CRU_UI_API UiEventArgs : public Object {
 public:
  UiEventArgs(Object* sender, Object* original_sender)
      : sender_(sender), original_sender_(original_sender), handled_(false) {}

  UiEventArgs(const UiEventArgs& other) = default;
  UiEventArgs(UiEventArgs&& other) = default;
  UiEventArgs& operator=(const UiEventArgs& other) = default;
  UiEventArgs& operator=(UiEventArgs&& other) = default;
  ~UiEventArgs() override = default;

  Object* GetSender() const { return sender_; }

  Object* GetOriginalSender() const { return original_sender_; }

  bool IsHandled() const { return handled_; }
  void SetHandled(const bool handled = true) { handled_ = handled; }

 private:
  Object* sender_;
  Object* original_sender_;
  bool handled_;
};
}  // namespace cru::ui::event
