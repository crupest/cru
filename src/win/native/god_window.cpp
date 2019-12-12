#include "cru/win/native/god_window.hpp"

#include "cru/common/logger.hpp"
#include "cru/win/native/exception.hpp"
#include "cru/win/native/ui_application.hpp"
#include "cru/win/native/window_class.hpp"
#include "god_window_message.hpp"
#include "timer.hpp"

namespace cru::platform::native::win {
constexpr auto god_window_class_name = L"GodWindowClass";

LRESULT CALLBACK GodWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  const auto app = WinUiApplication::GetInstance();

  if (app) {
    LRESULT result;
    const auto handled = app->GetGodWindow()->HandleGodWindowMessage(
        hWnd, uMsg, wParam, lParam, &result);
    if (handled)
      return result;
    else
      return DefWindowProcW(hWnd, uMsg, wParam, lParam);
  } else
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

GodWindow::GodWindow(WinUiApplication* application) {
  application_ = application;

  const auto h_instance = application->GetInstanceHandle();

  god_window_class_ = std::make_unique<WindowClass>(god_window_class_name,
                                                    GodWndProc, h_instance);

  hwnd_ = CreateWindowEx(0, god_window_class_name, L"", 0, CW_USEDEFAULT,
                         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                         HWND_MESSAGE, nullptr, h_instance, nullptr);

  if (hwnd_ == nullptr)
    throw Win32Error(::GetLastError(), "Failed to create god window.");
}

GodWindow::~GodWindow() {
  if (!::DestroyWindow(hwnd_)) {
    // Although this could be "safely" ignore.
    log::Warn("Failed to destroy god window.");
  }
}

bool GodWindow::HandleGodWindowMessage(HWND hwnd, UINT msg, WPARAM w_param,
                                       LPARAM l_param, LRESULT* result) {
  switch (msg) {
    case invoke_later_message_id: {
      const auto p_action = reinterpret_cast<std::function<void()>*>(w_param);
      (*p_action)();
      delete p_action;
      *result = 0;
      return true;
    }
    case WM_TIMER: {
      const auto id = static_cast<UINT_PTR>(w_param);
      const auto action = application_->GetTimerManager()->GetAction(id);
      if (action.has_value()) {
        (action.value().second)();
        if (!action.value().first)
          application_->GetTimerManager()->KillTimer(id);
        result = 0;
        return true;
      }
      break;
    }
    default:
      return false;
  }
  return false;
}
}  // namespace cru::platform::native::win
