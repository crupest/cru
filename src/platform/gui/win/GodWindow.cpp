#include "cru/platform/gui/win/GodWindow.h"

#include "cru/base/log/Logger.h"
#include "cru/platform/gui/win/Exception.h"
#include "cru/platform/gui/win/UiApplication.h"
#include "cru/platform/gui/win/WindowClass.h"

namespace cru::platform::gui::win {
constexpr auto god_window_class_name = L"GodWindowClass";

LRESULT CALLBACK GodWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  const auto app = WinUiApplication::GetInstance();

  if (app) {
    LRESULT result;
    auto god_window = app->GetGodWindow();
    if (god_window != nullptr) {
      const auto handled = god_window->HandleGodWindowMessage(
          hWnd, uMsg, wParam, lParam, &result);
      if (handled) return result;
    }
  }
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
    CRU_LOG_TAG_WARN("Failed to destroy god window.");
  }
}

bool GodWindow::HandleGodWindowMessage(HWND hwnd, UINT msg, WPARAM w_param,
                                       LPARAM l_param, LRESULT* result) {
  WindowNativeMessageEventArgs args(
      WindowNativeMessage{hwnd, msg, w_param, l_param});
  message_event_.Raise(args);

  if (args.IsHandled()) {
    *result = args.GetResult();
    return true;
  }

  return false;
}
}  // namespace cru::platform::gui::win
