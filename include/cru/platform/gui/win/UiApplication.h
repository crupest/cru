#pragma once
#include "Resource.h"

#include "cru/platform/gui/Base.h"
#include "cru/platform/gui/UiApplication.h"

#include <memory>

namespace cru::platform::graphics::direct2d {
class DirectGraphicsFactory;
}

namespace cru::platform::gui::win {
class CRU_WIN_GUI_API WinUiApplication : public WinNativeResource,
                                         public virtual IUiApplication {
 public:
  static WinUiApplication* GetInstance() { return instance; }

 private:
  static WinUiApplication* instance;

 public:
  WinUiApplication();

  CRU_DELETE_COPY(WinUiApplication)
  CRU_DELETE_MOVE(WinUiApplication)

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

  cru::platform::graphics::direct2d::DirectGraphicsFactory*
  GetDirectFactory() {
    return graph_factory_.get();
  }

  ICursorManager* GetCursorManager() override;

  IClipboard* GetClipboard() override;

  HINSTANCE GetInstanceHandle() const { return instance_handle_; }

  GodWindow* GetGodWindow() const { return god_window_.get(); }
  TimerManager* GetTimerManager() const { return timer_manager_.get(); }
  WindowManager* GetWindowManager() const { return window_manager_.get(); }

 private:
  HINSTANCE instance_handle_;

  bool is_quit_on_all_window_closed_ = true;

  std::unique_ptr<cru::platform::graphics::direct2d::DirectGraphicsFactory>
      graph_factory_;

  std::unique_ptr<GodWindow> god_window_;
  std::unique_ptr<TimerManager> timer_manager_;
  std::unique_ptr<WindowManager> window_manager_;

  std::unique_ptr<WinCursorManager> cursor_manager_;
  std::unique_ptr<WinClipboard> clipboard_;

  std::vector<std::function<void()>> quit_handlers_;
};
}  // namespace cru::platform::gui::win
