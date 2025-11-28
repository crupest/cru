#pragma once

#include <cru/platform/gui/Input.h>

#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_keyboard.h>

namespace cru::platform::gui::sdl {
MouseButton ConvertMouseButton(Uint8 button);
KeyModifier ConvertKeyModifier(SDL_Keymod keymod);
KeyModifier GetKeyModifier();
}  // namespace cru::platform::gui::sdl
