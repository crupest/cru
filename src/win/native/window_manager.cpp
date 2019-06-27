#include "window_manager.hpp"

#include "cru/win/native/ui_application.hpp"
#include "cru/win/native/native_window.hpp"
#include "cru/win/native/window_class.hpp"

#include <assert.h>

namespace cru::platform::native::win {
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
  general_window_class_ = std::make_shared<WindowClass>(
      L"CruUIWindowClass", GeneralWndProc, application->GetInstanceHandle());
}

WindowManager::~WindowManager() {
  for (const auto [key, window] : window_map_) delete window;
}

void WindowManager::RegisterWindow(HWND hwnd, WinNativeWindow* window) {
  assert(window_map_.count(hwnd) == 0);  // The hwnd is already in the map.
  window_map_.emplace(hwnd, window);
}

void WindowManager::UnregisterWindow(HWND hwnd) {
  const auto find_result = window_map_.find(hwnd);
  assert(find_result != window_map_.end());  // The hwnd is not in the map.
  window_map_.erase(find_result);
  if (window_map_.empty()) application_->Quit(0);
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
  for (auto [key, value] : window_map_) windows.push_back(value);
  return windows;
}
}  // namespace cru::platform::native::win
