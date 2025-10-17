#include "cru/platform/gui/xcb/Cursor.h"
#include "cru/base/Exception.h"
#include "cru/platform/gui/xcb/UiApplication.h"

#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>
#include <memory>

namespace cru::platform::gui::xcb {
XcbCursor::XcbCursor(XcbUiApplication* application, xcb_cursor_t cursor,
                     bool auto_free)
    : application_(application), cursor_(cursor), auto_free_(auto_free) {}

XcbCursor::~XcbCursor() {
  if (auto_free_) {
    xcb_free_cursor(application_->GetXcbConnection(), cursor_);
  }
}

xcb_cursor_t XcbCursor::GetXcbCursor() { return cursor_; }

XcbCursorManager::XcbCursorManager(XcbUiApplication* application)
    : application_(application) {
  auto code = xcb_cursor_context_new(application->GetXcbConnection(),
                                     application->GetFirstXcbScreen(),
                                     &xcb_cursor_context_);
  if (code != 0) {
    throw PlatformException("Failed to call xcb_cursor_context_new.");
  }

  cursors_[SystemCursorType::Arrow] =
      std::make_shared<XcbCursor>(application_, XCB_CURSOR_NONE, false);
  cursors_[SystemCursorType::Hand] = LoadXCursor("pointer");
  cursors_[SystemCursorType::IBeam] = LoadXCursor("ibeam");
}

XcbCursorManager::~XcbCursorManager() {
  xcb_cursor_context_free(xcb_cursor_context_);
}

std::shared_ptr<ICursor> XcbCursorManager::GetSystemCursor(
    SystemCursorType type) {
  return cursors_[type];
}

std::shared_ptr<XcbCursor> XcbCursorManager::LoadXCursor(
    std::string_view name) {
  return std::make_shared<XcbCursor>(
      application_, xcb_cursor_load_cursor(xcb_cursor_context_, name.data()),
      true);
}

}  // namespace cru::platform::gui::xcb
