#pragma once
#include "pre.hpp"

#include <Windows.h>

#include "ui_event.hpp"

namespace cru::ui::events {
struct WindowNativeMessage {
  HWND hwnd;
  int msg;
  WPARAM w_param;
  LPARAM l_param;
};

class WindowNativeMessageEventArgs : public UiEventArgs {
 public:
  WindowNativeMessageEventArgs(Object* sender, Object* original_sender,
                               const WindowNativeMessage& message)
      : UiEventArgs(sender, original_sender),
        message_(message),
        result_(std::nullopt) {}
  WindowNativeMessageEventArgs(const WindowNativeMessageEventArgs& other) =
      default;
  WindowNativeMessageEventArgs(WindowNativeMessageEventArgs&& other) = default;
  WindowNativeMessageEventArgs& operator=(
      const WindowNativeMessageEventArgs& other) = default;
  WindowNativeMessageEventArgs& operator=(
      WindowNativeMessageEventArgs&& other) = default;
  ~WindowNativeMessageEventArgs() override = default;

  WindowNativeMessage GetWindowMessage() const { return message_; }

  std::optional<LRESULT> GetResult() const { return result_; }

  void SetResult(const std::optional<LRESULT> result) { result_ = result; }

 private:
  WindowNativeMessage message_;
  std::optional<LRESULT> result_;
};
}
