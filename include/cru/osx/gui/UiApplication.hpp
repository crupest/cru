#pragma once
#include "Resource.hpp"
#include "cru/platform/gui/UiApplication.hpp"

#include <functional>
#include <memory>

namespace cru::platform::gui::osx {
class OsxWindow;

namespace details {
class OsxUiApplicationPrivate;
}

class OsxUiApplication : public OsxGuiResource, public virtual IUiApplication {
  friend details::OsxUiApplicationPrivate;
  friend OsxWindow;

 public:
  OsxUiApplication();

  CRU_DELETE_COPY(OsxUiApplication)
  CRU_DELETE_MOVE(OsxUiApplication)

  ~OsxUiApplication() override;

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

  IMenu* GetApplicationMenu() override;

  std::optional<String> ShowSaveDialog(SaveDialogOptions options) override;

  std::optional<std::vector<String>> ShowOpenDialog(
      OpenDialogOptions options) override;

 private:
  void UnregisterWindow(OsxWindow* window);

  std::unique_ptr<details::OsxUiApplicationPrivate> p_;
};
}  // namespace cru::platform::gui::osx
