#pragma once
#include "cru/platform/win/win_pre_config.hpp"

#include "cru/common/base.hpp"

#include <map>
#include <memory>
#include <vector>

namespace cru::platform::win {
class WinApplication;
class WinNativeWindow;
class WindowClass;

class WindowManager : public Object {
 public:
  WindowManager(WinApplication* application);
  WindowManager(const WindowManager& other) = delete;
  WindowManager(WindowManager&& other) = delete;
  WindowManager& operator=(const WindowManager& other) = delete;
  WindowManager& operator=(WindowManager&& other) = delete;
  ~WindowManager() override = default;

  // Get the general window class for creating ordinary window.
  std::shared_ptr<WindowClass> GetGeneralWindowClass() const {
    return general_window_class_;
  }

  // Register a window newly created.
  // This function adds the hwnd to hwnd-window map.
  // It should be called immediately after a window was created.
  void RegisterWindow(HWND hwnd, WinNativeWindow* window);

  // Unregister a window that is going to be destroyed.
  // This function removes the hwnd from the hwnd-window map.
  // It should be called immediately before a window is going to be destroyed,
  void UnregisterWindow(HWND hwnd);

  // Return a pointer to the Window object related to the HWND or nullptr if the
  // hwnd is not in the map.
  WinNativeWindow* FromHandle(HWND hwnd);

  std::vector<WinNativeWindow*> GetAllWindows() const;

 private:
  WinApplication* application_;

  std::shared_ptr<WindowClass> general_window_class_;
  std::map<HWND, WinNativeWindow*> window_map_;
};

}  // namespace cru::platform::win
