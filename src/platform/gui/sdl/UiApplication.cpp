
#include "cru/platform/gui/sdl/UiApplication.h"

#include "cru/base/Base.h"
#include "cru/platform/graphics/cairo/CairoGraphicsFactory.h"
#include "cru/platform/gui/sdl/Window.h"

#include <algorithm>

namespace cru::platform::gui::sdl {
SdlUiApplication::SdlUiApplication(
    graphics::cairo::CairoGraphicsFactory *cairo_factory) {
  release_cairo_factory_ = false;
  if (cairo_factory == nullptr) {
    cairo_factory = new graphics::cairo::CairoGraphicsFactory();
    release_cairo_factory_ = true;
  }
  cairo_factory_ = cairo_factory;
}

SdlUiApplication::~SdlUiApplication() {
  if (release_cairo_factory_) {
    delete cairo_factory_;
  }
}

graphics::cairo::CairoGraphicsFactory *SdlUiApplication::GetCairoFactory() {
  return cairo_factory_;
}

int SdlUiApplication::Run() {
  auto exit_code = event_loop_.Run();

  for (const auto &handler : this->quit_handlers_) {
    handler();
  }

  return exit_code;
}

void SdlUiApplication::RequestQuit(int quit_code) {
  event_loop_.RequestQuit(quit_code);
}

void SdlUiApplication::AddOnQuitHandler(std::function<void()> handler) {
  this->quit_handlers_.push_back(std::move(handler));
}

bool SdlUiApplication::IsQuitOnAllWindowClosed() {
  return is_quit_on_all_window_closed_;
}

void SdlUiApplication::SetQuitOnAllWindowClosed(
    bool quit_on_all_window_closed) {
  is_quit_on_all_window_closed_ = quit_on_all_window_closed;
}

long long SdlUiApplication::SetImmediate(std::function<void()> action) {
  return event_loop_.SetImmediate(std::move(action));
}

long long SdlUiApplication::SetTimeout(std::chrono::milliseconds milliseconds,
                                       std::function<void()> action) {
  return event_loop_.SetTimeout(std::move(action), std::move(milliseconds));
}

long long SdlUiApplication::SetInterval(std::chrono::milliseconds milliseconds,
                                        std::function<void()> action) {
  return event_loop_.SetInterval(std::move(action), std::move(milliseconds));
}

void SdlUiApplication::CancelTimer(long long id) {
  return event_loop_.CancelTimer(static_cast<int>(id));
}

std::vector<INativeWindow *> SdlUiApplication::GetAllWindow() {
  std::vector<INativeWindow *> windows(windows_.size());
  std::ranges::copy(windows_, windows.begin());
  return windows;
}

INativeWindow *SdlUiApplication::CreateWindow() { return new SdlWindow(this); }

cru::platform::graphics::IGraphicsFactory *
SdlUiApplication::GetGraphicsFactory() {
  return cairo_factory_;
}

ICursorManager *SdlUiApplication::GetCursorManager() { NotImplemented(); }

IClipboard *SdlUiApplication::GetClipboard() { NotImplemented(); }

IMenu *SdlUiApplication::GetApplicationMenu() { return nullptr; }

void SdlUiApplication::RegisterWindow(SdlWindow *window) {
  windows_.push_back(window);
}

void SdlUiApplication::UnregisterWindow(SdlWindow *window) {
  std::erase(windows_, window);
}
}  // namespace cru::platform::gui::sdl
