#pragma once
#include "Base.h"

#include "cru/base/Bitmask.h"
#include "cru/platform/gui/Menu.h"

#include "SaveOpenDialogOptions.h"

#include <chrono>
#include <functional>
#include <memory>
#include <vector>

namespace cru::platform::gui {
// The entry point of a ui application.
struct CRU_PLATFORM_GUI_API IUiApplication : public virtual IPlatformResource {
 public:
  static IUiApplication* GetInstance();

 protected:
  IUiApplication();

 public:
  ~IUiApplication() override;

  // Block current thread and run the message loop. Return the exit code when
  // message loop gets a quit message (possibly posted by method RequestQuit).
  virtual int Run() = 0;

  // Post a quit message with given quit code.
  virtual void RequestQuit(int quit_code) = 0;

  virtual void AddOnQuitHandler(std::function<void()> handler) = 0;

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
   * \todo Implement on Windows/X11.
   */
  virtual std::optional<String> ShowSaveDialog(SaveDialogOptions options);

  /**
   * \todo Implement on Windows/X11.
   */
  virtual std::optional<std::vector<String>> ShowOpenDialog(
      OpenDialogOptions options);
};
}  // namespace cru::platform::gui
