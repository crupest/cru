#pragma once
#include "Base.hpp"

#include <memory>

namespace cru::platform::native::win {
class GodWindow : public Object {
  CRU_DEFINE_CLASS_LOG_TAG("cru::platform::native::win::GodWindow")

 public:
  explicit GodWindow(WinUiApplication* application);

  CRU_DELETE_COPY(GodWindow)
  CRU_DELETE_MOVE(GodWindow)

  ~GodWindow() override;

  HWND GetHandle() const { return hwnd_; }

  bool HandleGodWindowMessage(HWND hwnd, UINT msg, WPARAM w_param,
                              LPARAM l_param, LRESULT* result);

 private:
  WinUiApplication* application_;

  std::unique_ptr<WindowClass> god_window_class_;
  HWND hwnd_;
};
}  // namespace cru::platform::native::win
