#include "cru/platform/gui/sdl/Input.h"

namespace cru::platform::gui::sdl {
MouseButton ConvertMouseButton(Uint8 button) {
  switch (button) {
    case SDL_BUTTON_LEFT:
      return MouseButtons::Left;
    case SDL_BUTTON_MIDDLE:
      return MouseButtons::Middle;
    case SDL_BUTTON_RIGHT:
      return MouseButtons::Right;
  }
  return MouseButtons::Unknown;
}

KeyModifier ConvertKeyModifier(SDL_Keymod keymod) {
  KeyModifier result;
  if (keymod & SDL_KMOD_SHIFT) result |= KeyModifiers::Shift;
  if (keymod & SDL_KMOD_CTRL) result |= KeyModifiers::Ctrl;
  if (keymod & SDL_KMOD_ALT) result |= KeyModifiers::Alt;
  return result;
}

KeyModifier GetKeyModifier() { return ConvertKeyModifier(SDL_GetModState()); }
}  // namespace cru::cru::platform::gui::sdl
