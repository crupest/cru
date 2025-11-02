
#pragma once
#include "../UiApplication.h"
#include "Base.h"

#include <cru/base/Timer.h>
#include <cru/platform/graphics/Factory.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>

namespace cru::platform::gui::sdl {
class SdlWindow;

class SdlUiApplication : public SdlResource, public virtual IUiApplication {
  friend SdlWindow;

 public:
  explicit SdlUiApplication(graphics::IGraphicsFactory* graphics_factory, bool release_graphics_factory);
  ~SdlUiApplication();

 public:
  int Run() override;

  void RequestQuit(int quit_code) override;

  void AddOnQuitHandler(std::function<void()> handler) override;

  bool IsQuitOnAllWindowClosed() override;
  void SetQuitOnAllWindowClosed(bool quit_on_all_window_closed) override;

  long long SetImmediate(std::function<void()> action) override;
  long long SetTimeout(std::chrono::milliseconds milliseconds,
                       std::function<void()> action) override;
  long long SetInterval(std::chrono::milliseconds milliseconds,
                        std::function<void()> action) override;
  void CancelTimer(long long id) override;

  std::vector<INativeWindow*> GetAllWindow() override;

  INativeWindow* CreateWindow() override;

  cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory() override;

  ICursorManager* GetCursorManager() override;

  IClipboard* GetClipboard() override;

  // If return nullptr, it means the menu is not supported.
  IMenu* GetApplicationMenu() override;

 private:
  void RegisterWindow(SdlWindow* window);
  void UnregisterWindow(SdlWindow* window);
  void RunOnMainThread(std::function<void()> action);
  void PostEmptyEvent();
  long long SetTimer(std::chrono::milliseconds milliseconds,
                     std::function<void()> action, bool repeat);

 private:
  graphics::IGraphicsFactory* graphics_factory_;
  bool release_graphics_factory_;

  std::uint32_t empty_event_type_;
  TimerRegistry<std::function<void()>> timers_;
  std::atomic_int quit_code_;
  std::vector<std::function<void()>> quit_handlers_;

  bool is_quit_on_all_window_closed_;
  std::vector<SdlWindow*> windows_;
};
}  // namespace cru::platform::gui::sdl
