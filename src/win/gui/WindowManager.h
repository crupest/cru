#pragma once
#include "cru/win/WinPreConfig.h"

#include "cru/common/Base.h"

#include <map>
#include <memory>
#include <vector>

namespace cru::platform::gui::win {
class WinUiApplication;
class WinNativeWindow;
class WindowClass;

class WindowManager : public Object {
 public:
  WindowManager(WinUiApplication* application);

  CRU_DELETE_COPY(WindowManager)
  CRU_DELETE_MOVE(WindowManager)

  ~WindowManager() override;

  // Get the general window class for creating ordinary window.
  WindowClass* GetGeneralWindowClass() const {
    return general_window_class_.get();
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
  WinUiApplication* application_;

  std::unique_ptr<WindowClass> general_window_class_;
  std::map<HWND, WinNativeWindow*> window_map_;
};
}  // namespace cru::platform::gui::win
