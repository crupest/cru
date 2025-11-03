#pragma once
#include "Base.h"

#include <cru/platform/gui/Cursor.h>

#include <memory>

namespace cru::platform::gui::osx {
namespace details {
class OsxWindowPrivate;
class OsxCursorPrivate;
class OsxCursorManagerPrivate;
}  // namespace details

class OsxCursor : public OsxGuiResource, public virtual ICursor {
  friend class OsxWindow;
  friend class details::OsxWindowPrivate;

 public:
  OsxCursor(IUiApplication* ui_application, SystemCursorType cursor_type);
  ~OsxCursor() override;

 private:
  std::unique_ptr<details::OsxCursorPrivate> p_;
};

class OsxCursorManager : public OsxGuiResource, public virtual ICursorManager {
 public:
  explicit OsxCursorManager(IUiApplication* ui_application);
  ~OsxCursorManager() override;

  std::shared_ptr<ICursor> GetSystemCursor(SystemCursorType type) override;

 private:
  std::unique_ptr<details::OsxCursorManagerPrivate> p_;
};
}  // namespace cru::platform::gui::osx
