#include "cru/osx/gui/Window.hpp"

#include "cru/osx/gui/UiApplication.hpp"

namespace cru::platform::gui::osx {
namespace details {
class OsxWindowPrivate {
  friend OsxWindow;

 public:
  explicit OsxWindowPrivate(OsxWindow* osx_window) : osx_window_(osx_window) {}

  CRU_DELETE_COPY(OsxWindowPrivate)
  CRU_DELETE_MOVE(OsxWindowPrivate)

  ~OsxWindowPrivate() = default;

 private:
  OsxWindow* osx_window_;
};
}

OsxWindow::OsxWindow(OsxUiApplication* ui_application)
    : OsxGuiResource(ui_application), p_(new details::OsxWindowPrivate(this)) {}

OsxWindow::~OsxWindow() {}
}
