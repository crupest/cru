#include "cru/platform/gui/sdl/UiApplication.h"

#include "cru/base/Base.h"
#include "cru/platform/graphics/Factory.h"
#include "cru/platform/gui/sdl/Base.h"
#include "cru/platform/gui/sdl/Window.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <algorithm>
#include <chrono>
#include <functional>

namespace cru::platform::gui::sdl {
SdlUiApplication::SdlUiApplication(graphics::IGraphicsFactory* graphics_factory,
                                   bool release_graphics_factory)
    : graphics_factory_(graphics_factory),
      release_graphics_factory_(release_graphics_factory),
      quit_code_(0) {
  CheckSdlReturn(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS));
  empty_event_type_ = SDL_RegisterEvents(1);
}

SdlUiApplication::~SdlUiApplication() {
  SDL_Quit();

  if (release_graphics_factory_) {
    delete graphics_factory_;
  }
}

int SdlUiApplication::Run() {
  while (true) {
    auto now = std::chrono::steady_clock::now();

    if (auto result = timers_.Update(now)) {
      result->data();
      continue;
    }

    auto timeout = timers_.NextTimeout(now);

    SDL_Event event;
    CheckSdlReturn(timeout ? SDL_WaitEventTimeout(&event, timeout->count())
                           : SDL_WaitEvent(&event));
    if (event.type == SDL_EVENT_QUIT) {
      break;
    }
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

ICursorManager* SdlUiApplication::GetCursorManager() { NotImplemented(); }

IClipboard* SdlUiApplication::GetClipboard() { NotImplemented(); }

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
}  // namespace cru::platform::gui::sdl
