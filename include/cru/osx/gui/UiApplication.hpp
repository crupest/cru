#pragma once
#include "Resource.hpp"
#include "cru/platform/gui/UiApplication.hpp"

#include <functional>
#include <memory>

namespace cru::platform::gui::osx {
namespace details {
class OsxUiApplicationPrivate;
}

class OsxUiApplication : public OsxGuiResource, public virtual IUiApplication {
  friend details::OsxUiApplicationPrivate;

 public:
  OsxUiApplication();

  CRU_DELETE_COPY(OsxUiApplication)
  CRU_DELETE_MOVE(OsxUiApplication)

  ~OsxUiApplication() override;

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

  INativeWindow* CreateWindow(INativeWindow* parent,
                              CreateWindowFlag flags) override;

  cru::platform::graphics::IGraphicsFactory* GetGraphFactory() override;

  ICursorManager* GetCursorManager() override;

 private:
  std::unique_ptr<details::OsxUiApplicationPrivate> p_;
};
}  // namespace cru::platform::gui::osx
