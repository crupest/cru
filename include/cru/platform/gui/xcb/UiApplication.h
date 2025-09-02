#pragma once
#include "../UiApplication.h"
#include "Base.h"

#include <xcb/xcb.h>
#include <functional>

namespace cru::platform::gui::xcb {
class XcbUiApplication : public XcbResource, public virtual IUiApplication {
 public:
  XcbUiApplication();
  ~XcbUiApplication();

  void CheckXcbConnectionError();

  int Run() override;

  // Post a quit message with given quit code.
  virtual void RequestQuit(int quit_code) = 0;

  void AddOnQuitHandler(std::function<void()> handler) override;

  virtual bool IsQuitOnAllWindowClosed() = 0;
  virtual void SetQuitOnAllWindowClosed(bool quit_on_all_window_closed) = 0;

  // Timer id should always be positive (not 0) and never the same. So it's ok
  // to use negative value (or 0) to represent no timer.
  virtual long long SetImmediate(std::function<void()> action) = 0;
  virtual long long SetTimeout(std::chrono::milliseconds milliseconds,
                               std::function<void()> action) = 0;
  virtual long long SetInterval(std::chrono::milliseconds milliseconds,
                                std::function<void()> action) = 0;
  // Implementation should guarantee calls on timer id already canceled have no
  // effects and do not crash. Also canceling negative id or 0 should always
  // result in no-op.
  virtual void CancelTimer(long long id) = 0;

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

  std::vector<std::function<void()>> quit_handlers_;
  int exit_code_;
};
}  // namespace cru::platform::gui::xcb
