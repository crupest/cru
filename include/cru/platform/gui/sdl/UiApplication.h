
#pragma once
#include "../UiApplication.h"
#include "Base.h"

#include <cru/base/platform/unix/EventLoop.h>
#include <cru/platform/graphics/cairo/CairoGraphicsFactory.h>

#include <functional>

namespace cru::platform::gui::sdl {
class SdlWindow;

class SdlUiApplication : public SdlResource, public virtual IUiApplication {
  friend SdlWindow;

 public:
  explicit SdlUiApplication(
      graphics::cairo::CairoGraphicsFactory* cairo_factory = nullptr);
  ~SdlUiApplication();

 public:
  graphics::cairo::CairoGraphicsFactory* GetCairoFactory();

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

 private:
  graphics::cairo::CairoGraphicsFactory* cairo_factory_;
  bool release_cairo_factory_;

  cru::platform::unix::UnixEventLoop event_loop_;
  std::vector<std::function<void()>> quit_handlers_;

  bool is_quit_on_all_window_closed_;
  std::vector<SdlWindow*> windows_;
};
}  // namespace cru::platform::gui::xcb
