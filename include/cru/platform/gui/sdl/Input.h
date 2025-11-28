#pragma once

#include <cru/platform/gui/Input.h>

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

namespace cru::platform::gui::sdl {
MouseButton ConvertMouseButton(Uint8 button);
KeyCode ConvertKeyCode(SDL_Keycode keycode);
KeyModifier ConvertKeyModifier(SDL_Keymod keymod);
KeyModifier GetKeyModifier();
}  // namespace cru::platform::gui::sdl
