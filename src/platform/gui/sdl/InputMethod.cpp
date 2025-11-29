#include "cru/platform/gui/sdl/InputMethod.h"
#include "SDL3/SDL_events.h"
#include "cru/platform/gui/sdl/Base.h"
#include "cru/platform/gui/sdl/Window.h"

#include <SDL3/SDL_keyboard.h>

namespace cru::platform::gui::sdl {
SdlInputMethodContext::SdlInputMethodContext(SdlWindow* window)
    : window_(window) {}

bool SdlInputMethodContext::ShouldManuallyDrawCompositionText() {
  return false;
}

void SdlInputMethodContext::EnableIME() {
  if (auto sdl_window = window_->GetSdlWindow()) {
    CheckSdlReturn(SDL_StartTextInput(sdl_window));
  }
}
void SdlInputMethodContext::DisableIME() {
  if (auto sdl_window = window_->GetSdlWindow()) {
    CheckSdlReturn(SDL_StopTextInput(sdl_window));
  }
}

void SdlInputMethodContext::CompleteComposition() { CancelComposition(); }

void SdlInputMethodContext::CancelComposition() {
  if (auto sdl_window = window_->GetSdlWindow()) {
    CheckSdlReturn(SDL_ClearComposition(sdl_window));
  }
}

CompositionText SdlInputMethodContext::GetCompositionText() { return {}; }

void SdlInputMethodContext::SetCandidateWindowPosition(const Point& point) {
  if (auto sdl_window = window_->GetSdlWindow()) {
    SDL_Rect rect{static_cast<int>(point.x), static_cast<int>(point.y), 1, 1};
    CheckSdlReturn(SDL_SetTextInputArea(sdl_window, &rect, 0));
  }
}

namespace {
std::optional<SDL_WindowID> GetEventWindowId(const SDL_Event& event) {
  switch (event.type) {
    case SDL_EVENT_TEXT_INPUT:
      return event.text.windowID;
    default:
      return std::nullopt;
  }
}
}  // namespace

bool SdlInputMethodContext::HandleEvent(const SDL_Event* event) {
  if (window_->GetSdlWindowId() != GetEventWindowId(*event)) return false;

  if (event->type == SDL_EVENT_TEXT_INPUT) {
    std::string text(event->text.text);
    TextEvent_.Raise(text);
    return true;
  }
  return false;
}

}  // namespace cru::platform::gui::sdl
