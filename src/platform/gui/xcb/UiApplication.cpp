#include <cru/platform/gui/xcb/UiApplication.h>

#include <xcb/xcb.h>

namespace cru::platform::gui::xcb {
XcbUiApplication::XcbUiApplication() {
  int screen_num;
  xcb_connection_t *connection = xcb_connect(NULL, &screen_num);
  this->CheckXcbConnectionError();

  const xcb_setup_t *setup = xcb_get_setup(connection);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  this->screen_ = iter.data;
}

XcbUiApplication::~XcbUiApplication() { xcb_disconnect(this->xcb_); }

void XcbUiApplication::CheckXcbConnectionError() {
  if (xcb_connection_has_error(this->xcb_)) {
    throw XcbException("xcb_connection_has_error returned non-zero.");
  }
}
}  // namespace cru::platform::gui::xcb
