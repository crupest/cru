#include "cru/platform/gui/xcb/Cursor.h"
#include "cru/base/Base.h"
#include "cru/platform/gui/xcb/UiApplication.h"

#include <xcb/xcb.h>

namespace cru::platform::gui::xcb {
XcbCursor* XcbCursor::LoadXCursor(io::Stream* stream) { NotImplemented(); }

XcbCursor::XcbCursor(XcbUiApplication* application, xcb_cursor_t cursor,
                     bool auto_free)
    : application_(application), cursor_(cursor), auto_free_(auto_free) {}

XcbCursor::~XcbCursor() {
  if (auto_free_) {
    xcb_free_cursor(application_->GetXcbConnection(), cursor_);
  }
}

xcb_cursor_t XcbCursor::GetXcbCursor() { return cursor_; }

}  // namespace cru::platform::gui::xcb
