#include <cru/platform/gui/xcb/UiApplication.h>

#include <xcb/xcb.h>

namespace cru::platform::gui::xcb {
XcbUiApplication::XcbUiApplication() : exit_code_(0) {
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

int XcbUiApplication::Run() {

  // TODO: A Big Implement.

  for (const auto &handler : this->quit_handlers_) {
    handler();
  }

  return exit_code_;
}

void XcbUiApplication::AddOnQuitHandler(std::function<void()> handler) {
  this->quit_handlers_.push_back(std::move(handler));
}

long long XcbUiApplication::SetImmediate(std::function<void()> action) {
  return event_loop_.SetImmediate(std::move(action));
}

long long XcbUiApplication::SetTimeout(std::chrono::milliseconds milliseconds,
                                       std::function<void()> action) {
  return event_loop_.SetTimeout(std::move(action), std::move(milliseconds));
}

long long XcbUiApplication::SetInterval(std::chrono::milliseconds milliseconds,
                                        std::function<void()> action) {
  return event_loop_.SetInterval(std::move(action), std::move(milliseconds));
}

void XcbUiApplication::CancelTimer(long long id) {
  return event_loop_.CancelTimer(static_cast<int>(id));
}
}  // namespace cru::platform::gui::xcb
