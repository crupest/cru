#pragma once
#include "win_pre_config.hpp"

#include <memory>
#include <optional>

#include "cru/common/base.hpp"

namespace cru::platform::win {
class WinApplication;
class WindowClass;

class GodWindow : public Object {
 public:
  explicit GodWindow(WinApplication* application);
  GodWindow(const GodWindow& other) = delete;
  GodWindow(GodWindow&& other) = delete;
  GodWindow& operator=(const GodWindow& other) = delete;
  GodWindow& operator=(GodWindow&& other) = delete;
  ~GodWindow() override;

  HWND GetHandle() const { return hwnd_; }

  std::optional<LRESULT> HandleGodWindowMessage(HWND hwnd, int msg,
                                                WPARAM w_param, LPARAM l_param);

 private:
  WinApplication* application_;

  std::shared_ptr<WindowClass> god_window_class_;
  HWND hwnd_;
};
}  // namespace cru::platform::win