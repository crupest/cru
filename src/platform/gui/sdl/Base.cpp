#include "cru/platform/gui/sdl/Base.h"

#include <SDL3/SDL_error.h>

namespace cru::platform::gui::sdl {
void CheckSdlReturn(bool success) {
  if (!success) {
    throw SdlException(SDL_GetError());
  }
}
}
