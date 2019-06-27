#pragma once
#include "../win_pre_config.hpp"

#include "platform_id.hpp"

#include "cru/platform/native/ui_applicaition.hpp"

#include <memory>

namespace cru::platform::native::win {
class GodWindow;
class TimerManager;
class WindowManager;

class WinUiApplication : public UiApplication {
  friend UiApplication* UiApplication::CreateInstance();

 public:
  static WinUiApplication* GetInstance();

 private:
  explicit WinUiApplication(HINSTANCE h_instance);

 public:
  WinUiApplication(const WinUiApplication&) = delete;
  WinUiApplication(WinUiApplication&&) = delete;
  WinUiApplication& operator=(const WinUiApplication&) = delete;
  WinUiApplication& operator=(WinUiApplication&&) = delete;
  ~WinUiApplication() override;

  CRU_PLATFORMID_IMPLEMENT_WIN

 public:
  int Run() override;
  void Quit(int quit_code) override;

  void AddOnQuitHandler(const std::function<void()>& handler) override;

  void InvokeLater(const std::function<void()>& action) override;
  unsigned long SetTimeout(std::chrono::milliseconds milliseconds,
                           const std::function<void()>& action) override;
  unsigned long SetInterval(std::chrono::milliseconds milliseconds,
                            const std::function<void()>& action) override;
  void CancelTimer(unsigned long id) override;

  std::vector<NativeWindow*> GetAllWindow() override;
  NativeWindow* CreateWindow(NativeWindow* parent) override;

  bool IsAutoDelete() const { return auto_delete_; }
  void SetAutoDelete(bool value) { auto_delete_ = value; }

  HINSTANCE GetInstanceHandle() const { return h_instance_; }

  GodWindow* GetGodWindow() const { return god_window_.get(); }
  TimerManager* GetTimerManager() const { return timer_manager_.get(); }
  WindowManager* GetWindowManager() const { return window_manager_.get(); }

 private:
  bool auto_delete_ = false;

  HINSTANCE h_instance_;

  std::shared_ptr<GodWindow> god_window_;
  std::shared_ptr<TimerManager> timer_manager_;
  std::shared_ptr<WindowManager> window_manager_;

  std::vector<std::function<void()>> quit_handlers_;
};
}  // namespace cru::platform::native::win
