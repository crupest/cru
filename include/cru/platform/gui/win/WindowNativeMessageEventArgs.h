#pragma once
#include "Base.h"

#include "cru/common/Base.h"

namespace cru::platform::gui::win {
struct CRU_WIN_GUI_API WindowNativeMessage {
  HWND hwnd;
  UINT msg;
  WPARAM w_param;
  LPARAM l_param;
};

class CRU_WIN_GUI_API WindowNativeMessageEventArgs : public Object {
 public:
  WindowNativeMessageEventArgs(const WindowNativeMessage& message)
      : message_(message) {}
  CRU_DEFAULT_COPY(WindowNativeMessageEventArgs)
  CRU_DEFAULT_MOVE(WindowNativeMessageEventArgs)
  ~WindowNativeMessageEventArgs() override = default;

  const WindowNativeMessage& GetWindowMessage() const { return message_; }

  LRESULT GetResult() const { return result_; }
  void SetResult(LRESULT result) { result_ = result; }

  bool IsHandled() const { return handled_; }
  void SetHandled(bool handled) { handled_ = handled; }

  void HandleWithResult(LRESULT result) {
    handled_ = true;
    result_ = result;
  }

 private:
  WindowNativeMessage message_;
  LRESULT result_;
  bool handled_ = false;
};
}  // namespace cru::platform::gui::win
