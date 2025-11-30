#include "cru/platform/gui/sdl/Clipboard.h"
#include "cru/platform/gui/sdl/Base.h"

#include <SDL3/SDL_clipboard.h>

namespace cru::platform::gui::sdl {
std::string SdlClipboard::GetText() {
  auto text = SDL_GetClipboardText();
  std::string result(text);
  SDL_free(text);
  return result;
}

void SdlClipboard::SetText(std::string text) {
  CheckSdlReturn(SDL_SetClipboardText(text.c_str()));
}

}  // namespace cru::platform::gui::sdl
