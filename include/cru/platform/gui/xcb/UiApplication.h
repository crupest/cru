#pragma once
#include "../UiApplication.h"
#include "Base.h"

#include <cru/base/platform/unix/EventLoop.h>

#include <xcb/xcb.h>
#include <functional>

namespace cru::platform::gui::xcb {
class XcbUiApplication : public XcbResource, public virtual IUiApplication {
 public:
  XcbUiApplication();
  ~XcbUiApplication();

  void CheckXcbConnectionError();

  int Run() override;

  virtual void RequestQuit(int quit_code) = 0;

  void AddOnQuitHandler(std::function<void()> handler) override;

  virtual bool IsQuitOnAllWindowClosed() = 0;
  virtual void SetQuitOnAllWindowClosed(bool quit_on_all_window_closed) = 0;

  long long SetImmediate(std::function<void()> action) override;
  long long SetTimeout(std::chrono::milliseconds milliseconds,
                       std::function<void()> action) override;
  long long SetInterval(std::chrono::milliseconds milliseconds,
                        std::function<void()> action) override;
  void CancelTimer(long long id) override;

  virtual std::vector<INativeWindow*> GetAllWindow() = 0;

  virtual INativeWindow* CreateWindow() = 0;

  virtual cru::platform::graphics::IGraphicsFactory* GetGraphicsFactory() = 0;

  virtual ICursorManager* GetCursorManager() = 0;

  virtual IClipboard* GetClipboard() = 0;

  // If return nullptr, it means the menu is not supported.
  virtual IMenu* GetApplicationMenu();

  /**
   * \todo Implement on Windows.
   */
  virtual std::optional<String> ShowSaveDialog(SaveDialogOptions options);

  /**
   * \todo Implement on Windows.
   */
  virtual std::optional<std::vector<String>> ShowOpenDialog(
      OpenDialogOptions options);

 private:
  xcb_connection_t* xcb_;
  xcb_screen_t* screen_;

  cru::platform::unix::UnixEventLoop event_loop_;
  std::vector<std::function<void()>> quit_handlers_;
  int exit_code_;
};
}  // namespace cru::platform::gui::xcb
