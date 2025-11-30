#pragma once
#include "Base.h"

#include <cru/platform/gui/Cursor.h>

#include <SDL3/SDL_mouse.h>

namespace cru::platform::gui::sdl {
class SdlCursor : public SdlResource, public virtual ICursor {
 public:
  static std::shared_ptr<SdlCursor> CreateSystem(SDL_SystemCursor cursor);

  explicit SdlCursor(SDL_Cursor* cursor, bool auto_destroy = true);
  ~SdlCursor();

  SDL_Cursor* GetSdlCursor();

 private:
  SDL_Cursor* cursor_;
  bool auto_destroy_;
};

struct SdlCursorManager : public SdlResource, public virtual ICursorManager {
 public:
  SdlCursorManager();

  std::shared_ptr<ICursor> GetSystemCursor(SystemCursorType type) override;

 private:
  std::shared_ptr<SdlCursor> arrow_cursor_;
  std::shared_ptr<SdlCursor> hand_cursor_;
  std::shared_ptr<SdlCursor> ibeam_cursor_;
};
}  // namespace cru::platform::gui::sdl
