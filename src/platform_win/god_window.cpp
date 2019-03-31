#include "cru/platform/win/god_window.hpp"

#include "cru/platform/win/exception.hpp"
#include "cru/platform/win/win_application.hpp"
#include "cru/platform/win/window_class.hpp"
#include "god_window_message.hpp"
#include "timer.hpp"

namespace cru::platform::win {
constexpr auto god_window_class_name = L"GodWindowClass";

LRESULT CALLBACK GodWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  const auto app = WinApplication::GetInstance();

  if (app) {
    const auto result =
        app->GetGodWindow()->HandleGodWindowMessage(hWnd, uMsg, wParam, lParam);
    if (result.has_value())
      return result.value();
    else
      return DefWindowProcW(hWnd, uMsg, wParam, lParam);
  } else
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

GodWindow::GodWindow(WinApplication* application) {
  application_ = application;

  const auto h_instance = application->GetInstanceHandle();

  god_window_class_ = std::make_shared<WindowClass>(god_window_class_name,
                                                    GodWndProc, h_instance);

  hwnd_ = CreateWindowEx(0, god_window_class_name, L"", 0, CW_USEDEFAULT,
                         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                         HWND_MESSAGE, nullptr, h_instance, nullptr);

  if (hwnd_ == nullptr) throw Win32Error(::GetLastError(), "Failed to create god window.");
}

GodWindow::~GodWindow() { ::DestroyWindow(hwnd_); }

std::optional<LRESULT> GodWindow::HandleGodWindowMessage(HWND hwnd, int msg,
                                                         WPARAM w_param,
                                                         LPARAM l_param) {
  switch (msg) {
    case invoke_later_message_id: {
      const auto p_action = reinterpret_cast<std::function<void()>*>(w_param);
      (*p_action)();
      delete p_action;
      return 0;
    }
    case WM_TIMER: {
      const auto id = static_cast<UINT_PTR>(w_param);
      const auto action = application_->GetTimerManager()->GetAction(id);
      if (action.has_value()) {
        (action.value().second)();
        if (!action.value().first)
          application_->GetTimerManager()->KillTimer(id);
        return 0;
      }
      break;
    }
    default:
      return std::nullopt;
  }
  return std::nullopt;
}
}  // namespace cru::platform::win