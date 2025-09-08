#include "cru/platform/gui/xcb/Window.h"
#include "cru/platform/gui/xcb/UiApplication.h"

namespace cru::platform::gui::xcb {
XcbWindow::XcbWindow(XcbUiApplication* application)
    : application_(application) {
  application->RegisterWindow(this);
}

XcbWindow::~XcbWindow() { application_->UnregisterWindow(this); }
}  // namespace cru::platform::gui::xcb
