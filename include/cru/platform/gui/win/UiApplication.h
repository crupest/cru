#pragma once
#include "Base.h"
#include "Window.h"
#include "WindowClass.h"

#include <cru/base/Timer.h>
#include <cru/platform/gui/UiApplication.h>

#include <memory>

namespace cru::platform::graphics::direct2d {
class DirectGraphicsFactory;
}

namespace cru::platform::gui::win {
class CRU_WIN_GUI_API WinUiApplication : public WinNativeResource,
                                         public virtual IUiApplication {
  friend WinNativeWindow;

 public:
  static WinUiApplication* GetInstance() { return instance; }

 private:
  static WinUiApplication* instance;

 public:
  WinUiApplication();

  ~WinUiApplication() override;

 public:
  int Run() override;
  void RequestQuit(int quit_code) override;

  void AddOnQuitHandler(std::function<void()> handler) override;

  bool IsQuitOnAllWindowClosed() override {
    return is_quit_on_all_window_closed_;
  }
  void SetQuitOnAllWindowClosed(bool quit_on_all_window_closed) override {
    is_quit_on_all_window_closed_ = quit_on_all_window_closed;
  }

  long long SetImmediate(std::function<void()> action) override;
  long long SetTimeout(std::chrono::milliseconds milliseconds,
                       std::function<void()> action) override;
  long long SetInterval(std::chrono::milliseconds milliseconds,
                        std::function<void()> action) override;
  void CancelTimer(long long id) override;

  std::vector<INativeWindow*> GetAllWindow() override;
  INativeWindow* CreateWindow() override;

  cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory() override;

  cru::platform::graphics::direct2d::DirectGraphicsFactory* GetDirectFactory() {
    return graph_factory_.get();
  }

  ICursorManager* GetCursorManager() override;

  IClipboard* GetClipboard() override;

  HINSTANCE GetInstanceHandle() const { return instance_handle_; }
  std::vector<WinNativeWindow*> GetAllWinWindow();
  WinNativeWindow* FromHWND(HWND hwnd);

 private:
  WindowClass* GetGeneralWindowClass();
  void RegisterWindow(WinNativeWindow* window);
  void UnregisterWindow(WinNativeWindow* window);

 private:
  HINSTANCE instance_handle_;

  bool is_quit_on_all_window_closed_ = true;

  std::unique_ptr<cru::platform::graphics::direct2d::DirectGraphicsFactory>
      graph_factory_;

  TimerRegistry<std::function<void()>> timers_;

  std::unique_ptr<WindowClass> general_window_class_;
  std::vector<WinNativeWindow*> windows_;

  std::unique_ptr<WinCursorManager> cursor_manager_;
  std::unique_ptr<WinClipboard> clipboard_;

  std::vector<std::function<void()>> quit_handlers_;
};
}  // namespace cru::platform::gui::win
