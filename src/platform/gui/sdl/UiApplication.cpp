#include "cru/platform/gui/sdl/UiApplication.h"

#include "cru/base/Base.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/sdl/Base.h"
#include "cru/platform/gui/sdl/Clipboard.h"
#include "cru/platform/gui/sdl/Cursor.h"
#include "cru/platform/gui/sdl/Window.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <memory>
#include <optional>

namespace cru::platform::gui::sdl {
SdlUiApplication::SdlUiApplication(graphics::IGraphicsFactory* graphics_factory,
                                   bool release_graphics_factory)
    : graphics_factory_(graphics_factory),
      release_graphics_factory_(release_graphics_factory),
      quit_code_(0) {
  CheckSdlReturn(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));
  empty_event_type_ = SDL_RegisterEvents(1);

  cursor_manager_ = std::make_unique<SdlCursorManager>();
  clipboard_ = std::make_unique<SdlClipboard>();
}

SdlUiApplication::~SdlUiApplication() {
  SDL_Quit();

  delete_later_pool_.Clean();

  if (release_graphics_factory_) {
    delete graphics_factory_;
  }
}

int SdlUiApplication::Run() {
  while (true) {
    if (auto result = timers_.Update(std::chrono::steady_clock::now())) {
      result->data();
      continue;
    }

    auto timeout = timers_.NextTimeout(std::chrono::steady_clock::now());

    SDL_Event event;
    CheckSdlReturn(timeout ? SDL_WaitEventTimeout(&event, timeout->count())
                           : SDL_WaitEvent(&event));
    if (event.type == SDL_EVENT_QUIT) {
      break;
    }

    DispatchEvent(event);

    delete_later_pool_.Clean();
  }

  for (const auto& handler : this->quit_handlers_) {
    handler();
  }

  return quit_code_;
}

void SdlUiApplication::RequestQuit(int quit_code) {
  quit_code_ = quit_code;
  SDL_Event event;
  event.type = SDL_EVENT_QUIT;
  event.quit.timestamp = SDL_GetTicksNS();
  CheckSdlReturn(SDL_PushEvent(&event));
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
  return SetTimeout(std::chrono::milliseconds::zero(), std::move(action));
}

long long SdlUiApplication::SetTimeout(std::chrono::milliseconds milliseconds,
                                       std::function<void()> action) {
  return SetTimer(milliseconds, std::move(action), false);
}

long long SdlUiApplication::SetInterval(std::chrono::milliseconds milliseconds,
                                        std::function<void()> action) {
  return SetTimer(milliseconds, std::move(action), true);
}

void SdlUiApplication::CancelTimer(long long id) {
  return timers_.Remove(static_cast<int>(id));
}

void SdlUiApplication::DeleteLater(Object* object) {
  delete_later_pool_.Add(object);
}

std::vector<INativeWindow*> SdlUiApplication::GetAllWindow() {
  std::vector<INativeWindow*> windows(windows_.size());
  std::ranges::copy(windows_, windows.begin());
  return windows;
}

INativeWindow* SdlUiApplication::CreateWindow() { return new SdlWindow(this); }

cru::platform::graphics::IGraphicsFactory*
SdlUiApplication::GetGraphicsFactory() {
  return graphics_factory_;
}

ICursorManager* SdlUiApplication::GetCursorManager() {
  return cursor_manager_.get();
}

IClipboard* SdlUiApplication::GetClipboard() { return clipboard_.get(); }

IMenu* SdlUiApplication::GetApplicationMenu() { return nullptr; }

void SdlUiApplication::RegisterWindow(SdlWindow* window) {
  windows_.push_back(window);
}

void SdlUiApplication::UnregisterWindow(SdlWindow* window) {
  std::erase(windows_, window);
}

void SdlUiApplication::RunOnMainThread(std::function<void()> action) {
  auto p = new std::function<void()>(std::move(action));
  SDL_RunOnMainThread(
      [](void* userdata) {
        auto action = static_cast<std::function<void()>*>(userdata);
        (*action)();
        delete action;
      },
      p, false);
}

void SdlUiApplication::PostEmptyEvent() {
  SDL_Event event;
  SDL_zero(event);
  event.type = empty_event_type_;
  event.user.timestamp = SDL_GetTicksNS();
  SDL_PushEvent(&event);
}

long long SdlUiApplication::SetTimer(std::chrono::milliseconds milliseconds,
                                     std::function<void()> action,
                                     bool repeat) {
  PostEmptyEvent();
  return timers_.Add(std::move(action), milliseconds, repeat);
}

bool SdlUiApplication::DispatchEvent(const SDL_Event& event) {
  for (auto window : windows_) {
    if (window->HandleEvent(&event)) {
      return true;
    }
  }
  return false;
}

}  // namespace cru::platform::gui::sdl
