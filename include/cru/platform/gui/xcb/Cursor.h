#pragma once

#include <cru/base/io/Stream.h>
#include "../Cursor.h"
#include "Base.h"

#include <xcb/xcb.h>

namespace cru::platform::gui::xcb {
class XcbUiApplication;
class XcbCursor : public XcbResource, public virtual ICursor {
 public:
  /**
   * Specification at
   * https://www.x.org/archive/X11R7.7/doc/man/man3/Xcursor.3.xhtml.
   */
  static XcbCursor* LoadXCursor(io::Stream* stream);

  XcbCursor(XcbUiApplication* application, xcb_cursor_t cursor, bool auto_free);
  ~XcbCursor() override;

  xcb_cursor_t GetXcbCursor();

 private:
  XcbUiApplication* application_;
  xcb_cursor_t cursor_;
  bool auto_free_;
};

class XcbCursorManager : public XcbResource, public virtual ICursorManager {
  virtual std::shared_ptr<ICursor> GetSystemCursor(SystemCursorType type) = 0;
};
}  // namespace cru::platform::gui::xcb
