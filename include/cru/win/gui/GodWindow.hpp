#pragma once
#include "Base.hpp"

#include "WindowNativeMessageEventArgs.hpp"
#include "cru/common/Event.hpp"
#include "cru/common/String.hpp"

#include <memory>

namespace cru::platform::gui::win {
class CRU_WIN_GUI_API GodWindow : public Object {
  CRU_DEFINE_CLASS_LOG_TAG(u"cru::platform::gui::win::GodWindow")

 public:
  explicit GodWindow(WinUiApplication* application);

  CRU_DELETE_COPY(GodWindow)
  CRU_DELETE_MOVE(GodWindow)

  ~GodWindow() override;

  HWND GetHandle() const { return hwnd_; }

  bool HandleGodWindowMessage(HWND hwnd, UINT msg, WPARAM w_param,
                              LPARAM l_param, LRESULT* result);

  IEvent<WindowNativeMessageEventArgs&>* MessageEvent() {
    return &message_event_;
  }

 private:
  WinUiApplication* application_;

  std::unique_ptr<WindowClass> god_window_class_;
  HWND hwnd_;

  Event<WindowNativeMessageEventArgs&> message_event_;
};
}  // namespace cru::platform::gui::win
