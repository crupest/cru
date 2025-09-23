#pragma once

#include <cru/base/io/Stream.h>
#include "../Cursor.h"
#include "Base.h"

#include <xcb/xcb.h>
#include <xcb/xcb_cursor.h>
#include <memory>
#include <string_view>
#include <unordered_map>

namespace cru::platform::gui::xcb {
class XcbUiApplication;

class XcbCursor : public XcbResource, public virtual ICursor {
 public:
  XcbCursor(XcbUiApplication* application, xcb_cursor_t cursor, bool auto_free);
  ~XcbCursor() override;

  xcb_cursor_t GetXcbCursor();

 private:
  XcbUiApplication* application_;
  xcb_cursor_t cursor_;
  bool auto_free_;
};

class XcbCursorManager : public XcbResource, public virtual ICursorManager {
 public:
  explicit XcbCursorManager(XcbUiApplication* application);
  ~XcbCursorManager() override;

  std::shared_ptr<ICursor> GetSystemCursor(SystemCursorType type) override;

 private:
  std::shared_ptr<XcbCursor> LoadXCursor(std::string_view name);

 private:
  XcbUiApplication* application_;
  xcb_cursor_context_t* xcb_cursor_context_;
  std::unordered_map<SystemCursorType, std::shared_ptr<XcbCursor>> cursors_;
};
}  // namespace cru::platform::gui::xcb
