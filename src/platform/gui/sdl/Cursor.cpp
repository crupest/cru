#include "cru/platform/gui/sdl/Cursor.h"
#include "cru/base/Base.h"
#include "cru/platform/gui/Cursor.h"
#include "cru/platform/gui/sdl/Base.h"

#include <memory>

namespace cru::platform::gui::sdl {
std::shared_ptr<SdlCursor> SdlCursor::CreateSystem(SDL_SystemCursor cursor) {
  auto c = SDL_CreateSystemCursor(cursor);
  if (!c) {
    throw SdlException("Failed to create system cursor.");
  }
  return std::make_shared<SdlCursor>(c, true);
}

SdlCursor::SdlCursor(SDL_Cursor* cursor, bool auto_destroy)
    : cursor_(cursor), auto_destroy_(auto_destroy) {}

SdlCursor::~SdlCursor() {
  if (auto_destroy_) {
    SDL_DestroyCursor(cursor_);
  }
}

SDL_Cursor* SdlCursor::GetSdlCursor() { return cursor_; }

SdlCursorManager::SdlCursorManager() {
  arrow_cursor_ = SdlCursor::CreateSystem(SDL_SYSTEM_CURSOR_DEFAULT);
  hand_cursor_ = SdlCursor::CreateSystem(SDL_SYSTEM_CURSOR_POINTER);
  ibeam_cursor_ = SdlCursor::CreateSystem(SDL_SYSTEM_CURSOR_TEXT);
}

std::shared_ptr<ICursor> SdlCursorManager::GetSystemCursor(
    SystemCursorType type) {
  switch (type) {
    case SystemCursorType::Arrow:
      return std::static_pointer_cast<ICursor>(arrow_cursor_);
    case SystemCursorType::Hand:
      return std::static_pointer_cast<ICursor>(hand_cursor_);
    case SystemCursorType::IBeam:
      return std::static_pointer_cast<ICursor>(ibeam_cursor_);
    default:
      UnreachableCode();
  }
}

}  // namespace cru::platform::gui::sdl
