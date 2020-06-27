#pragma once
#include "Resource.hpp"

#include "cru/platform/native/Cursor.hpp"

#include <memory>

namespace cru::platform::native::win {
class WinCursor : public WinNativeResource, public virtual ICursor {
 public:
  WinCursor(HCURSOR handle, bool auto_destroy);

  CRU_DELETE_COPY(WinCursor)
  CRU_DELETE_MOVE(WinCursor)

  ~WinCursor() override;

 public:
  HCURSOR GetHandle() const { return handle_; }

 private:
  HCURSOR handle_;
  bool auto_destroy_;
};

class WinCursorManager : public WinNativeResource,
                         public virtual ICursorManager {
 public:
  WinCursorManager();

  CRU_DELETE_COPY(WinCursorManager)
  CRU_DELETE_MOVE(WinCursorManager)

  ~WinCursorManager() override = default;

 public:
  std::shared_ptr<WinCursor> GetSystemWinCursor(SystemCursorType type);

  std::shared_ptr<ICursor> GetSystemCursor(SystemCursorType type) override {
    return std::static_pointer_cast<ICursor>(GetSystemWinCursor(type));
  }

 private:
  std::shared_ptr<WinCursor> sys_arrow_;
  std::shared_ptr<WinCursor> sys_hand_;
};
}  // namespace cru::platform::native::win
