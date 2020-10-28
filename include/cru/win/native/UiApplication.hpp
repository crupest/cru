#pragma once
#include "Resource.hpp"

#include "cru/platform/native/Base.hpp"
#include "cru/platform/native/UiApplication.hpp"

#include <memory>

namespace cru::platform::graph::win::direct {
class DirectGraphFactory;
}

namespace cru::platform::native::win {
class WinUiApplication : public WinNativeResource,
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

  long long SetImmediate(std::function<void()> action) override;
  long long SetTimeout(std::chrono::milliseconds milliseconds,
                       std::function<void()> action) override;
  long long SetInterval(std::chrono::milliseconds milliseconds,
                        std::function<void()> action) override;
  void CancelTimer(long long id) override;

  std::vector<INativeWindow*> GetAllWindow() override;
  INativeWindow* CreateWindow(INativeWindow* parent) override;

  cru::platform::graph::IGraphFactory* GetGraphFactory() override;

  cru::platform::graph::win::direct::DirectGraphFactory* GetDirectFactory() {
    return graph_factory_.get();
  }

  ICursorManager* GetCursorManager() override;

  HINSTANCE GetInstanceHandle() const { return instance_handle_; }

  GodWindow* GetGodWindow() const { return god_window_.get(); }
  TimerManager* GetTimerManager() const { return timer_manager_.get(); }
  WindowManager* GetWindowManager() const { return window_manager_.get(); }

 private:
  HINSTANCE instance_handle_;

  std::unique_ptr<cru::platform::graph::win::direct::DirectGraphFactory>
      graph_factory_;

  std::unique_ptr<GodWindow> god_window_;
  std::unique_ptr<TimerManager> timer_manager_;
  std::unique_ptr<WindowManager> window_manager_;

  std::unique_ptr<WinCursorManager> cursor_manager_;

  std::vector<std::function<void()>> quit_handlers_;
};
}  // namespace cru::platform::native::win
