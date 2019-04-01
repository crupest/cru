#pragma once
#include "win_pre_config.hpp"

#include "../ui_applicaition.hpp"
#include "cru/common/base.hpp"

#include <memory>

namespace cru::platform::win {
class GodWindow;
class TimerManager;
class WindowManager;
class GraphManager;
class WinGraphFactory;

class WinApplication : public Object, public virtual UiApplication {
 public:
  static WinApplication* GetInstance();

 private:
  static WinApplication* instance_;

 private:
  explicit WinApplication(HINSTANCE h_instance);

 public:
  WinApplication(const WinApplication&) = delete;
  WinApplication(WinApplication&&) = delete;
  WinApplication& operator=(const WinApplication&) = delete;
  WinApplication& operator=(WinApplication&&) = delete;
  ~WinApplication() override;

 public:
  int Run() override;
  void Quit(int quit_code) override;

  void InvokeLater(const std::function<void()>& action) override;
  unsigned long SetTimeout(std::chrono::milliseconds milliseconds,
                           const std::function<void()>& action) override;
  unsigned long SetInterval(std::chrono::milliseconds milliseconds,
                            const std::function<void()>& action) override;
  void CancelTimer(unsigned long id) override;

  GraphFactory* GetGraphFactory() override;


  HINSTANCE GetInstanceHandle() const { return h_instance_; }

  GodWindow* GetGodWindow() const { return god_window_.get(); }
  TimerManager* GetTimerManager() const { return timer_manager_.get(); }
  WindowManager* GetWindowManager() const { return window_manager_.get(); }
  GraphManager* GetGraphManager() const { return graph_manager_.get(); }
  WinGraphFactory* GetWinGraphFactory() const { return graph_factory_.get(); }

 private:
  HINSTANCE h_instance_;

  std::shared_ptr<GodWindow> god_window_;
  std::shared_ptr<TimerManager> timer_manager_;
  std::shared_ptr<WindowManager> window_manager_;
  std::shared_ptr<GraphManager> graph_manager_;
  std::shared_ptr<WinGraphFactory> graph_factory_;
};
}  // namespace cru::platform::win
