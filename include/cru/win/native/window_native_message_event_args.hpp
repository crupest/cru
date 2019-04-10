#pragma once
#include "../win_pre_config.hpp"

#include "cru/common/base.hpp"

namespace cru::win::native {
struct WindowNativeMessage {
  HWND hwnd;
  UINT msg;
  WPARAM w_param;
  LPARAM l_param;
};

class WindowNativeMessageEventArgs : public Object {
 public:
  WindowNativeMessageEventArgs(const WindowNativeMessage& message)
      : message_(message) {}
  WindowNativeMessageEventArgs(const WindowNativeMessageEventArgs& other) =
      default;
  WindowNativeMessageEventArgs(WindowNativeMessageEventArgs&& other) = default;
  WindowNativeMessageEventArgs& operator=(
      const WindowNativeMessageEventArgs& other) = default;
  WindowNativeMessageEventArgs& operator=(
      WindowNativeMessageEventArgs&& other) = default;
  ~WindowNativeMessageEventArgs() override = default;

  WindowNativeMessage GetWindowMessage() const { return message_; }

  LRESULT GetResult() const { return result_; }
  void SetResult(LRESULT result) { result_ = result; }

  bool IsHandled() const { return handled_; }
  void SetHandled(bool handled) { handled_ = handled; }

  void HandledAndSetResult(LRESULT result) {
    handled_ = true;
    result_ = result;
  }

 private:
  WindowNativeMessage message_;
  LRESULT result_;
  bool handled_ = false;
};
}  // namespace cru::win::native
