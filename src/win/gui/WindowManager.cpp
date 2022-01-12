#include "WindowManager.hpp"

#include "cru/win/gui/UiApplication.hpp"
#include "cru/win/gui/Window.hpp"
#include "cru/win/gui/WindowClass.hpp"

namespace cru::platform::gui::win {
LRESULT __stdcall GeneralWndProc(HWND hWnd, UINT Msg, WPARAM wParam,
                                 LPARAM lParam) {
  auto window =
      WinUiApplication::GetInstance()->GetWindowManager()->FromHandle(hWnd);

  LRESULT result;
  if (window != nullptr &&
      window->HandleNativeWindowMessage(hWnd, Msg, wParam, lParam, &result))
    return result;

  return DefWindowProc(hWnd, Msg, wParam, lParam);
}

WindowManager::WindowManager(WinUiApplication* application) {
  application_ = application;
  general_window_class_ = std::make_unique<WindowClass>(
      L"CruUIWindowClass", GeneralWndProc, application->GetInstanceHandle());
}

WindowManager::~WindowManager() {
  for (const auto& [key, window] : window_map_) delete window;
}

void WindowManager::RegisterWindow(HWND hwnd, WinNativeWindow* window) {
  Expects(window_map_.count(hwnd) == 0);  // The hwnd is already in the map.
  window_map_.emplace(hwnd, window);
}

void WindowManager::UnregisterWindow(HWND hwnd) {
  const auto find_result = window_map_.find(hwnd);
  Expects(find_result != window_map_.end());  // The hwnd is not in the map.
  window_map_.erase(find_result);
  if (window_map_.empty() && application_->IsQuitOnAllWindowClosed())
    application_->RequestQuit(0);
}

WinNativeWindow* WindowManager::FromHandle(HWND hwnd) {
  const auto find_result = window_map_.find(hwnd);
  if (find_result == window_map_.end())
    return nullptr;
  else
    return find_result->second;
}

std::vector<WinNativeWindow*> WindowManager::GetAllWindows() const {
  std::vector<WinNativeWindow*> windows;
  for (const auto& [key, value] : window_map_) windows.push_back(value);
  return windows;
}
}  // namespace cru::platform::gui::win
