#pragma once
#include "../UiApplication.h"
#include "Base.h"

#include <cru/base/platform/unix/EventLoop.h>

#include <xcb/xcb.h>
#include <functional>

namespace cru::platform::gui::xcb {
class XcbWindow;

class XcbUiApplication : public XcbResource, public virtual IUiApplication {
  friend XcbWindow;

 public:
  XcbUiApplication();
  ~XcbUiApplication();

 public:
  void CheckXcbConnectionError();
  xcb_connection_t* GetXcbConnection();

  // This API is weird, but before we have correct screen API, we still use it.
  xcb_screen_t* GetFirstXcbScreen();

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
  void HandleXEvents();

  void RegisterWindow(XcbWindow* window);
  void UnregisterWindow(XcbWindow* window);

 private:
  xcb_connection_t* xcb_connection_;
  xcb_screen_t* screen_;

  cru::platform::unix::UnixEventLoop event_loop_;
  std::vector<std::function<void()>> quit_handlers_;

  bool is_quit_on_all_window_closed_;
  std::vector<XcbWindow*> windows_;
};
}  // namespace cru::platform::gui::xcb
