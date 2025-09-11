#include "cru/platform/gui/xcb/UiApplication.h"

#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#include "cru/platform/gui/xcb/Window.h"

#include <poll.h>
#include <xcb/xcb.h>

namespace cru::platform::gui::xcb {
XcbUiApplication::XcbUiApplication(
    graphics::cairo::CairoGraphicsFactory *cairo_factory) {
  release_cairo_factory_ = false;
  if (cairo_factory == nullptr) {
    cairo_factory = new graphics::cairo::CairoGraphicsFactory();
    release_cairo_factory_ = true;
  }
  cairo_factory_ = cairo_factory;

  is_quit_on_all_window_closed_ = false;

  int screen_num;
  xcb_connection_t *connection = xcb_connect(NULL, &screen_num);
  this->CheckXcbConnectionError();

  event_loop_.SetPoll(xcb_get_file_descriptor(connection), POLLIN,
                      [this](auto) { HandleXEvents(); });

  const xcb_setup_t *setup = xcb_get_setup(connection);
  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
  this->screen_ = iter.data;
}

XcbUiApplication::~XcbUiApplication() {
  xcb_disconnect(this->xcb_connection_);
  if (release_cairo_factory_) {
    delete cairo_factory_;
  }
}

void XcbUiApplication::CheckXcbConnectionError() {
  if (xcb_connection_has_error(this->xcb_connection_)) {
    throw XcbException("xcb_connection_has_error returned non-zero.");
  }
}

xcb_connection_t *XcbUiApplication::GetXcbConnection() {
  return xcb_connection_;
}

xcb_screen_t *XcbUiApplication::GetFirstXcbScreen() { return screen_; }

xcb_atom_t XcbUiApplication::GetOrCreateXcbAtom(std::string name) {
  auto iter = xcb_atom_.find(name);
  if (iter != xcb_atom_.cend()) {
    return iter->second;
  }

  auto cookie =
      xcb_intern_atom(xcb_connection_, false, name.size(), name.data());
  auto reply = xcb_intern_atom_reply(xcb_connection_, cookie, nullptr);
  auto atom = reply->atom;
  xcb_atom_.emplace(std::move(name), atom);
  return atom;
}

xcb_atom_t XcbUiApplication::GetXcbAtom_NET_WM_WINDOW_TYPE() {
  return GetOrCreateXcbAtom("_NET_WM_WINDOW_TYPE");
}

xcb_atom_t XcbUiApplication::GetXcbAtom_NET_WM_WINDOW_TYPE_NORMAL() {
  return GetOrCreateXcbAtom("_NET_WM_WINDOW_TYPE_NORMAL");
}

xcb_atom_t XcbUiApplication::GetXcbAtom_NET_WM_WINDOW_TYPE_UTILITY() {
  return GetOrCreateXcbAtom("_NET_WM_WINDOW_TYPE_UTILITY");
}

int XcbUiApplication::Run() {
  auto exit_code = event_loop_.Run();

  for (const auto &handler : this->quit_handlers_) {
    handler();
  }

  return exit_code;
}

void XcbUiApplication::RequestQuit(int quit_code) {
  event_loop_.RequestQuit(quit_code);
}

void XcbUiApplication::AddOnQuitHandler(std::function<void()> handler) {
  this->quit_handlers_.push_back(std::move(handler));
}

bool XcbUiApplication::IsQuitOnAllWindowClosed() {
  return is_quit_on_all_window_closed_;
}

void XcbUiApplication::SetQuitOnAllWindowClosed(
    bool quit_on_all_window_closed) {
  is_quit_on_all_window_closed_ = quit_on_all_window_closed;
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

void XcbUiApplication::HandleXEvents() {
  xcb_generic_event_t *event;
  while ((event = xcb_poll_for_event(xcb_connection_))) {
    auto event_xcb_window = XcbWindow::GetEventWindow(event);
    for (auto window : windows_) {
      if (window->GetXcbWindow() == event_xcb_window) {
        window->HandleEvent(event);
        break;
      }
    }
    ::free(event);
  }
}

void XcbUiApplication::RegisterWindow(XcbWindow *window) {
  windows_.push_back(window);
}

void XcbUiApplication::UnregisterWindow(XcbWindow *window) {
  std::erase(windows_, window);
}
}  // namespace cru::platform::gui::xcb
