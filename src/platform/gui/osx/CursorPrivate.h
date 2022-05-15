#pragma once
#include "cru/platform/gui/osx/Cursor.h"

#import <AppKit/NSCursor.h>

namespace cru::platform::gui::osx {
class OsxWindow;

namespace details {
class OsxWindowPrivate;

class OsxCursorPrivate {
  friend OsxWindow;
  friend OsxWindowPrivate;

 public:
  OsxCursorPrivate(OsxCursor* cursor, SystemCursorType cursor_type);

  CRU_DELETE_COPY(OsxCursorPrivate)
  CRU_DELETE_MOVE(OsxCursorPrivate)

  ~OsxCursorPrivate();

 private:
  OsxCursor* cursor_;
  NSCursor* ns_cursor_;
};
}  // namespace details
}  // namespace cru::platform::gui::osx
